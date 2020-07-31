import ntpath
import sys
from sys import path

import yaml
from kubernetes import client, config
from kubernetes.client.rest import ApiException
from kubernetes.stream import stream
import tarfile
from tempfile import TemporaryFile
import time
import os
from pprint import pprint
import uuid


def copy_files_to_volume(sources, mountFolder, persistentVolumeClaimName, namespace, targetFolder=None, state=None):

    if state:
        print(state.kubeconfig)
        config.load_kube_config(state.kubeconfig)
    configuration = client.Configuration()
    configuration.verify_ssl = False
    api_client = client.ApiClient(configuration)
    api_instance = client.CoreV1Api(api_client)

    pod_name = f"copy-pod"
    resp = None
    try:
        resp = api_instance.read_namespaced_pod(name=pod_name, namespace=namespace)
        pprint(resp)
    except ApiException as e:
        if e.status != 404:
            print("Unknown error: %s" % e, file=sys.stderr)
            exit(1)

    if not resp:
        print("Creating temporary pod to copy files in volume")
        ## VolumeList
        volumeList = []
        v1PersistentVolumeClaim = client.V1PersistentVolumeClaimVolumeSource(claim_name=persistentVolumeClaimName)
        v1Volume = client.V1Volume(name="workdir", persistent_volume_claim=v1PersistentVolumeClaim)
        volumeList.append(v1Volume)

        # containerList
        containers = []
        v1VolumeMountList = []
        if targetFolder != mountFolder:
            subPath = targetFolder.replace(mountFolder, '')
            if subPath[0] == "/":
                subPath = subPath[1:]
            v1VolumeMount = client.V1VolumeMount(name="workdir", mount_path=targetFolder, sub_path=subPath)
        else:
            v1VolumeMount = client.V1VolumeMount(name="workdir", mount_path=mountFolder)
        v1VolumeMountList.append(v1VolumeMount)
        v1container = client.V1Container(name="redis", image="redis", volume_mounts=v1VolumeMountList)
        containers.append(v1container)
        v1PodSpec = client.V1PodSpec(volumes=volumeList, containers=containers, host_network=True, dns_policy="Default")
        metadata = client.V1ObjectMeta(name=pod_name, namespace=namespace)
        body = client.V1Pod(metadata=metadata, spec=v1PodSpec, kind="Pod")  # V1Pod |

        # prettyPrint
        pretty = True
        try:
            api_response = api_instance.create_namespaced_pod(namespace=namespace, body=body, pretty=pretty)
            timeout = 30  # [seconds]
            timeout_start = time.time()
            while time.time() < timeout_start + timeout:
                pod_status_response = api_instance.patch_namespaced_pod_status(pod_name, namespace, body, pretty=pretty)
                pprint("Status is " + pod_status_response.status.phase + ". Waiting for pod to be created")
                if pod_status_response.status.phase == "Running":
                    pprint(pod_status_response)
                    break
                # retry every 2 seconds
                time.sleep(2)

        except ApiException as e:
            print("Exception when creating copy-pod: %s\n" % e, file=sys.stderr)
            exit(1)

    print("Copying files in pod")
    exec_command = ['tar', 'xvf', '-', '-C', '/']

    for source in sources:
        resp = stream(api_instance.connect_get_namespaced_pod_exec, pod_name, namespace,
                      command=exec_command,
                      stderr=True,
                      stdin=True,
                      stdout=True,
                      tty=False,
                      _preload_content=False)

        pprint(resp)

        source_filename = ntpath.basename(source)
        destination_file = os.path.join(targetFolder, source_filename)
        pprint(f"destination_file:   {destination_file}")
        with TemporaryFile() as tar_buffer:
            with tarfile.open(fileobj=tar_buffer, mode='w') as tar:
                tar.add(name=source, arcname=destination_file)

            tar_buffer.seek(0)
            commands = [tar_buffer.read()]

            while resp.is_open():
                resp.update(timeout=1)
                if resp.peek_stdout():
                    print(f"STDOUT: file copied in {destination_file}")
                if resp.peek_stderr():
                    print("STDERR: %s" % resp.read_stderr())
                if commands:
                    c = commands.pop(0)
                    print("Running command... %s\n" % c.decode())
                    resp.write_stdin(c)
                else:
                    break
            resp.close()

    time.sleep(10)
    # delete pod
    print("Deleting temporary pod", file=sys.stderr)
    pretty = True  # str | If 'true', then the output is pretty printed. (optional)
    grace_period_seconds = 0  # int | The duration in seconds before the object should be deleted. Value must be non-negative integer. The value zero indicates delete immediately. If this value is nil, the default grace period for the specified type will be used. Defaults to a per object value if not specified. zero means delete immediately. (optional)
    orphan_dependents = True  # bool | Deprecated: please use the PropagationPolicy, this field will be deprecated in 1.7. Should the dependent objects be orphaned. If true/false, the \"orphan\" finalizer will be added to/removed from the object's finalizers list. Either this field or PropagationPolicy may be set, but not both. (optional)
    # propagation_policy = 'propagation_policy_example'  # str | Whether and how garbage collection will be performed. Either this field or OrphanDependents may be set, but not both. The default policy is decided by the existing finalizer set in the metadata.finalizers and the resource-specific default policy. Acceptable values are: 'Orphan' - orphan the dependents; 'Background' - allow the garbage collector to delete the dependents in the background; 'Foreground' - a cascading policy that deletes all dependents in the foreground. (optional)
    body = client.V1DeleteOptions()  # V1DeleteOptions |  (optional)

    try:
        api_response = api_instance.delete_namespaced_pod(name=pod_name,
                                                          namespace=namespace,
                                                          pretty=pretty,
                                                          grace_period_seconds=grace_period_seconds,
                                                          orphan_dependents=orphan_dependents,
                                                          body=body)
    except ApiException as e:
        print("Exception when calling CoreV1Api->delete_namespaced_pod: %s\n" % e, file=sys.stderr)

    timeout = 30  # [seconds]
    timeout_start = time.time()
    while time.time() < timeout_start + timeout:
        try:
            pod_status_response = api_instance.patch_namespaced_pod_status(pod_name, namespace, body, pretty=pretty)
            pprint("Status is " + pod_status_response.status.phase + ". Waiting for pod to be deleted")
            # retry every 2 seconds
            time.sleep(2)
        except ApiException as e:
            if "\"reason\": \"NotFound\"" in str(e):
                print(f"{pod_name} Successfully deleted")
            break


def getCwlWorkflowId(cwl_document):
    print("parsing cwl")
    with open(cwl_document, 'r') as stream:
        try:
            graph = yaml.load(stream, Loader=yaml.FullLoader)["$graph"]
        except yaml.YAMLError as exc:
            print(exc, file=sys.stderr)

    for item in graph:
        if item.get('class') == "Workflow":
            return item.get('id')