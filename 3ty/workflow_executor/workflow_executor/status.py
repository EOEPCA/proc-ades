from pprint import pprint
import time
import kubernetes.client
from kubernetes.client.rest import ApiException
from kubernetes import client, config


def run(namespace, workflow_name, state=None):
    # # Setup K8 configs
    config.load_kube_config()

    # create an instance of the API class
    api_client = kubernetes.client.ApiClient()
    api_instance = kubernetes.client.BatchV1Api(api_client)
    pretty = True

    print("## JOB STATUS")

    try:
        api_response = api_instance.read_namespaced_job_status(name=workflow_name,namespace= namespace, pretty=pretty)

        if api_response.status.active:
            status = {"status": "Running", "error": ""}
            pprint(status)
            return status
        elif api_response.status.succeeded:
            status = {"status": "Success", "error": ""}
            pprint(status)
            return status
        elif api_response.status.failed:
            status = {"status": "Failed", "error": "Failed job"}
            pprint(status)
            return status

    except ApiException as e:
        print("Exception when calling get status: %s\n" % e)
        raise e

