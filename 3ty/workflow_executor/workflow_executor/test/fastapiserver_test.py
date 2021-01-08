import os
import unittest
from os import path
from fastapi.testclient import TestClient
from workflow_executor import fastapiserver as app
from workflow_executor import clean
from pprint import pprint
from pytest_kind import KindCluster
import time


class FastApiTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        print("Creating kubernetes test cluster")
        cls.cluster = KindCluster("workflow-executor-test-cluster")
        cls.cluster.create()

    @classmethod
    def tearDownClass(cls):
        print("Deleting kubernetes test cluster")
        cls.cluster.delete()

    def setUp(self):
        self.client = TestClient(app.app)
        self.serviceID = "vegetation-index"
        self.runID = "abc123abc123"
        self.prepareId = "vegetation-indexabc123abc123"

        test_folder_path = os.path.dirname(__file__)
        cwl = path.join(test_folder_path, "vegetation-index.cwl")
        inputs = path.join(test_folder_path, "inputs.json")

        f = open(cwl, "r")
        self.cwl_content = f.read()
        f.close()

        f = open(inputs, "r")
        self.inputs_content = f.read()
        f.close()

        os.environ["STORAGE_HOST"] = "https://nx10438.your-storageshare.de/"
        os.environ["STORAGE_USERNAME"] = "eoepca-demo-storage"
        os.environ["STORAGE_APIKEY"] = "FakeApiKey"
        os.environ["IMAGE_PULL_SECRETS"]="imagepullsecrets.json"

    def test_step1_get_root(self):
        response = self.client.get("/")
        assert response.status_code == 200
        assert response.json() == {"Hello": "World"}

    def test_step2_post_prepare(self):
        response = self.client.post("/prepare",
                                    json={"runID": self.runID, "serviceID": self.serviceID, "cwl": self.cwl_content})
        assert response.status_code == 201
        assert response.json() == {'prepareID': self.prepareId}

    def test_step3_get_prepare(self):
        response = self.client.get(f"/prepare/{self.prepareId}")

        attempts = 0
        while response.status_code != 200 and attempts < 3:
            attempts += 1
            response = self.client.get(f"/prepare/{self.prepareId}")
            time.sleep(1)

    def test_step4_post_execute(self):
        content = {"runID": self.runID, "serviceID": self.serviceID, "prepareID": self.prepareId,
                   "cwl": self.cwl_content, "inputs": self.inputs_content}

        pprint(content)

        # this delay/retry is a workaround
        # for further details check https://github.com/kubernetes/kubernetes/issues/66689#issuecomment-460748704
        for i in range(0, 2):
            while True:
                response = self.client.post("/execute", json=content)
                if response.status_code == 403:
                    print("response 403, retrying in 2 seconds")
                    time.sleep(2)
                    continue
                break

        assert response.status_code == 201

    #        assert response.json() == {'prepareID': self.prepareId}
    #        assert response.status_code == 200

    def test_step5_post_clean(self):
        clean.run(self.prepareId)


if __name__ == '__main__':
    unittest.main()
