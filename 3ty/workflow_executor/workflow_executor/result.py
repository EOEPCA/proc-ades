import sys
import time
from os import path
from pprint import pprint

import yaml
from jinja2 import Template
from kubernetes import client
from kubernetes.client.rest import ApiException

from workflow_executor import helpers


def run(namespace, mount_folder, volume_name_prefix, workflowname, outputfile, state=None):
    # create an instance of the API class
    apiclient = helpers.get_api_client()
    batch_api_instance = client.BatchV1Api(api_client=apiclient)
    core_api_instance = client.CoreV1Api(api_client=apiclient)

    # for further details check https://github.com/Duke-GCB/calrissian/blob/master/examples/ViewResultsJob.yaml
    jobname = f"{workflowname}-view-results"

    yamlFileTemplate = "CarlrissianViewResults.yaml"
    with open(path.join(path.dirname(__file__), yamlFileTemplate)) as f:

        print(f"Customizing stage-in job using the template {yamlFileTemplate} ")
        template = Template(f.read())
        variables = {
            "jobname": jobname,
            "mount_folder": mount_folder,
            "workflowname": workflowname,
            "outputfile": outputfile,
            "outputVolumeClaimName": f"{volume_name_prefix}-output-data",
        }

    yaml_modified = template.render(variables)

    body = yaml.safe_load(yaml_modified)
    pprint(body)

    try:
        resp = batch_api_instance.create_namespaced_job(body=body, namespace=namespace)
        print("Job created. status='%s'" % str(resp.status))
    except ApiException as e:
        print("Exception when submitting job: %s\n" % e, file=sys.stderr)
        return e

    # wait for job to finish
    count=0
    ret = batch_api_instance.read_namespaced_job(name=jobname, namespace=namespace)
    while not ret.status.conditions and count < 10:
        time.sleep(3)
        ret = batch_api_instance.read_namespaced_job(name=jobname, namespace=namespace)
        count += 1
    # get pod from job
    try:
        podlist = core_api_instance.list_namespaced_pod(namespace=namespace, label_selector=f"job-name={jobname}")
        pod = podlist.items[0].metadata.name
    except ApiException as e:
        print("Exception listing pods: %s\n" % e)
        raise e


    # get pod log
    try:
        print(f"Checking that pod {pod} is in ready state.")
        podReady=False
        count=0
        while not podReady and count < 10:
            time.sleep(3)
            podstatus = core_api_instance.read_namespaced_pod_status(name=pod, namespace=namespace)
            podReady = is_ready(podstatus=podstatus)
            count += 1

    except ApiException as e:
        print("Exception when checking pod status of pod %s : %s\n" % pod, e)
        raise e

    # get pod log
    try:
        print(f"retrieving logs of pod {pod}")
        result = core_api_instance.read_namespaced_pod_log(name=pod, namespace=namespace, container="view-results")
    except ApiException as e:
        print("Exception when retrieving result from output volume: %s\n" % e)
        raise e
    return eval(result)





def is_ready(podstatus) -> bool:
    """Check if the Pod is in the ready state.

    Returns:
        True if in the ready state; False otherwise.
    """

    # if there is no status, the pod is definitely not ready
    status = podstatus.status
    if status is None:
        return False

    # check the pod phase to make sure it is running. a pod in
    # the 'failed' or 'success' state will no longer be running,
    # so we only care if the pod is in the 'running' state.
    phase = status.phase
    if phase.lower() != "running":
        return False

    for cond in status.conditions:
        # we only care about the condition type 'ready'
        if cond.type.lower() != "ready":
            continue

        # check that the readiness condition is True
        return cond.status.lower() == "true"

    # Catchall
    return False