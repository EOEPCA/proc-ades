from pprint import pprint
import time
import kubernetes.client
from kubernetes.client.rest import ApiException
from kubernetes import client, config


def run(namespace, workflow_name, state=None):
    # # Setup K8 configs
    if state:
        config.load_kube_config(state.kubeconfig)
    configuration = client.Configuration()
    configuration.verify_ssl = False

    # create an instance of the API class
    api_client = kubernetes.client.ApiClient(configuration)
    api_instance = kubernetes.client.BatchV1Api(api_client)
    pretty = True

    # v1 = client.CoreV1Api(api_client)
    # ret = v1.list_namespaced_pod(watch=False, namespace=namespace)
    # error_pod_name = ""
    # for i in ret.items:
    #
    #     if i.status.phase == "Running":
    #         status = {"status": "Running", "error": ""}
    #         pprint(status)
    #         return 0
    #     elif i.status.phase == "Pending":
    #         status = {"status": "Running", "error": ""}
    #         pprint(status)
    #         return 0
    #     elif i.status.phase == "Failed":
    #         error_pod_name = i.metadata.name
    try:
        api_response = api_instance.read_namespaced_job_status(workflow_name, namespace, pretty=pretty)

        if api_response.status.active:
            status = {"status": "Running", "error": ""}
            pprint(status)
            return 0
        elif api_response.status.succeeded:
            status = {"status": "Success", "error": ""}
            pprint(status)
            return 0
        elif api_response.status.failed:
            status = {"status": "Failed", "error": "Failed job"}
            pprint(status)
            return 1

    except ApiException as e:
        print("Exception when calling get status: %s\n" % e)
        return 0
