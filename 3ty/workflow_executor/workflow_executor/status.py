import kubernetes.client
from kubernetes.client.rest import ApiException
from kubernetes import client


def run(namespace, jobname):
    # create an instance of the API class
    configuration = client.Configuration()
    api_instance = kubernetes.client.BatchV1Api(kubernetes.client.ApiClient(configuration))
    pretty = True

    try:
        api_response = api_instance.read_namespaced_job_status(jobname, namespace, pretty=pretty)

        if api_response.active:
            print("Running")
        elif api_response.status.failed:
            print("Failed")
        elif api_response.status.succeeded:
            print("Success")

    except ApiException as e:
        print("Exception when calling get status: %s\n" % e)
