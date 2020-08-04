import json
import time
from os import path
from kubernetes import client, config
from kubernetes.client.rest import ApiException
from kubernetes.stream import stream
from workflow_executor import helpers
import sys

def run(namespace, mount_folder, volume_name_prefix, workflowname, state=None):
    try:

        outputJson = path.join(mount_folder,"output-data",f"{workflowname}-stac-output.out")

        # create an instance of the API class
        if state:
            config.load_kube_config(state.kubeconfig)
        configuration = client.Configuration()
        configuration.verify_ssl = False
        api_instance = client.CoreV1Api(client.ApiClient(configuration))

        name = 'busybox-pod'
        resp = None
        try:
            resp = api_instance.read_namespaced_pod(name=name, namespace=namespace)
        except ApiException as e:
            if e.status != 404:
                print("Unknown error: %s" % e, file=sys.stderr)
                exit(1)

        if not resp:
            print("Pod %s does not exist. Creating it..." % name)
            pod_manifest = {
                'apiVersion': 'v1',
                'kind': 'Pod',
                'metadata': {
                    'name': name,
                },
                'spec': {
                    'volumes': [{
                        'name': f"{workflowname}",
                        'persistentVolumeClaim': {'claimName': f"{volume_name_prefix}-output-data",
                                                  'readOnly': True}
                    }],
                    'containers': [{
                        'image': 'busybox',
                        'name': 'sleep',
                        "args": [
                            "/bin/sh",
                            "-c",
                            "while true;do date;sleep 2; done"
                        ],
                        "volumeMounts": [{
                            'mountPath': f"{mount_folder}",
                            'name': f"{workflowname}"
                        }]
                    }]
                }
            }
            resp = api_instance.create_namespaced_pod(body=pod_manifest, namespace=namespace)
            while True:
                resp = api_instance.read_namespaced_pod(name=name, namespace=namespace)
                if resp.status.phase != 'Pending':
                    break
                time.sleep(1)
            print("Done.", file=sys.stderr)

        # Calling exec and waiting for response
        exec_command = [
            '/bin/sh',
            '-c',
            f"cat {outputJson} >&2"]
        resp = stream(api_instance.connect_get_namespaced_pod_exec,
                      name,
                      namespace,
                      command=exec_command,
                      stderr=True, stdin=False,
                      stdout=True, tty=False)

        # this is correct to put it in stdout
        print(resp, file=sys.stdout)
    finally:
        resp = api_instance.delete_namespaced_pod(name=name, namespace=namespace)
