import os
from pprint import pprint
import time
import kubernetes.client
from kubernetes.client.rest import ApiException
from kubernetes import client, config
from workflow_executor import helpers


ADES_LOGS_PATH = "/var/www/_run/res"

def run(namespace, workflow_name, state=None):
    # create an instance of the API class
    apiclient = helpers.get_api_client()
    api_instance = client.BatchV1Api(api_client=apiclient)

    pretty = True

    print("## JOB STATUS")

    try:
        api_response = api_instance.read_namespaced_job_status(name=workflow_name, namespace=namespace, pretty=pretty)

        if api_response.status.active:
            status = {"status": "Running", "error": ""}
            pprint(status)
            return status
        else:
            controller_uid = api_response.metadata.labels["controller-uid"]
            logs = helpers.retrieveLogs(controller_uid, namespace)
            helpers.storeLogs(logs, os.path.join(ADES_LOGS_PATH, f"{namespace}_calrissian.log"))

        # if processing has finished, store logs in /var/www/html/res
        if api_response.status.succeeded:
            status = {"status": "Success", "error": ""}
        elif api_response.status.failed:
            status = {"status": "Failed", "error": api_response.status.conditions[0].message}

        pprint(status)
        return status

    except ApiException as e:
        print("Exception when calling get status: %s\n" % e)
        raise e
