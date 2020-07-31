import ntpath
import sys
import time
import uuid
from os import path
from pprint import pprint

import yaml
from kubernetes import client, config
from kubernetes.client.rest import ApiException

from workflow_executor import helpers

package_directory = path.dirname(path.abspath(__file__))
__stagein_input_data_job_template = path.join(package_directory, 'StageInputDataJobTemplate.yaml')


def stac_stagein_run(namespace, volume_name_prefix, input_yaml, timeout, mountFolder, outputFolder, state=None):

    volume_name = volume_name_prefix + "-input-data"
    input_yaml_filename = ntpath.basename(input_yaml)
    yamlFileTemplate = __stagein_input_data_job_template

    # giving a unique name to stage in job
    job_name = f"stage-in-{uuid.uuid4()}"

    # copying input_yaml in volume -input-data
    helpers.copy_files_to_volume(sources=[input_yaml], mountFolder=mountFolder, targetFolder=outputFolder,
                                 persistentVolumeClaimName=volume_name,
                                 namespace=namespace,state=state)
    # Setup K8 configs
    if state:
        config.load_kube_config(state.kubeconfig)
    configuration = client.Configuration()
    configuration.verify_ssl=False
    api_instance_batch_v1_api = client.BatchV1Api(client.ApiClient(configuration))

    with open(path.join(path.dirname(__file__), yamlFileTemplate)) as f:

        print(f"Creating stage-in job using the template {yamlFileTemplate} ")
        # volume
        yaml_modified = f.read().replace("calrissian-input-data", volume_name)
        yaml_modified = yaml_modified.replace("stage-in.yaml", path.join(outputFolder, input_yaml_filename))
        yaml_modified = yaml_modified.replace("/tmp/staged", outputFolder)
        yaml_modified = yaml_modified.replace("name: stage-in-job", f"name: {job_name}")
        yaml_modified = yaml_modified.replace("mountPath: /tmp", f"mountPath: {mountFolder}")

        body = yaml.safe_load(yaml_modified)
        pprint(body)

        try:
            resp = api_instance_batch_v1_api.create_namespaced_job(body=body, namespace=namespace)
            print("Job created. status='%s'" % str(resp.status), file=sys.stderr)
        except ApiException as e:
            print("Exception when submitting job: %s\n" % e, file=sys.stderr)
            return 1

    pretty = True
    timeout_start = time.time()
    while time.time() < timeout_start + timeout:
        try:
            job_status_response = api_instance_batch_v1_api.read_namespaced_job_status(name=job_name,
                                                                                       namespace=namespace,
                                                                                       pretty=pretty)
            if not job_status_response.status.succeeded and not job_status_response.status.failed:
                pprint(f"{job_name} job is running", sys.stderr)
            else:

                if job_status_response.status.succeeded:
                    print("Stage-in job succeeded", file=sys.stderr)
                elif job_status_response.status.failed:
                    print("Stage-in job failed", file=sys.stderr)

                api_instance_core_v1_api = client.CoreV1Api(client.ApiClient(configuration))
                podlist = api_instance_core_v1_api.list_namespaced_pod(namespace=namespace,
                                                                       label_selector=f'job-name={job_name}',
                                                                       watch=False)
                for pod in podlist.items:
                    print("%s\t%s\t%s" % (pod.metadata.name,
                                          pod.status.phase,
                                          pod.status.pod_ip), file=sys.stderr)
                    if not pod.status.phase == "Pending":
                        api_response = api_instance_core_v1_api.read_namespaced_pod_log(name=pod.metadata.name,
                                                                                        namespace=namespace)
                        pprint(api_response)
                        return
                break
            # retry every 2 seconds
            time.sleep(4)

        except ApiException as e:
            print("Exception when submitting job: %s\n" % e, file=sys.stderr)
            return 1
