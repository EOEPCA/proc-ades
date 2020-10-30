import json
import time
from os import path
from pprint import pprint

from kubernetes import client, config
from kubernetes.client.rest import ApiException
from kubernetes.stream import stream
import sys


def run(namespace, mount_folder, volume_name_prefix, workflowname, outputfile,keepworkspace=False, state=None):
    try:

        # create an instance of the API class
        config.load_kube_config()
        api_instance = client.CoreV1Api(client.ApiClient())

        name = f"{workflowname}-copy-pod"
        resp = None
        try:
            resp = api_instance.read_namespaced_pod(name=name, namespace=namespace)
        except ApiException as e:
            if e.status != 404:
                print("Exception when reading copy-pod: %s\n" % e, file=sys.stderr)
                raise e

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
                        'name': f"{workflowname}-output-data",
                        'persistentVolumeClaim': {'claimName': f"{volume_name_prefix}-output-data"}
                    },{
                        'name': f"{workflowname}-input-data",
                        'persistentVolumeClaim': {'claimName': f"{volume_name_prefix}-input-data"}
                    },{
                        'name': f"{workflowname}-tmpout",
                        'persistentVolumeClaim': {'claimName': f"{volume_name_prefix}-tmpout"}
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
                            'mountPath': f"{mount_folder}/output-data",
                            'name': f"{workflowname}-output-data"
                        },
                        {
                            'mountPath': f"{mount_folder}/input-data",
                            'name': f"{workflowname}-input-data"
                        },
                        {
                            'mountPath': f"{mount_folder}/tmpout",
                            'name': f"{workflowname}-tmpout"
                        }]
                    }]
                }
            }

            print(f"pod_manifest: {pod_manifest}")
            resp = api_instance.create_namespaced_pod(body=pod_manifest, namespace=namespace)
            while True:
                resp = api_instance.read_namespaced_pod(name=name, namespace=namespace)
                if resp.status.phase != 'Pending':
                    break
                time.sleep(1)
            print("Done.")



        # Calling exec and waiting for response
        exec_command = [
            '/bin/sh',
            '-c',
            f"cat {mount_folder}/output-data/{workflowname}/*/{outputfile} >&2"]
        resp = stream(api_instance.connect_get_namespaced_pod_exec,
                      name,
                      namespace,
                      command=exec_command,
                      stderr=True, stdin=False,
                      stdout=True, tty=False)
        pprint(f"resp cat {mount_folder}/{workflowname}/*/{outputfile}      {resp}" )
        if "cat: can't open" in resp:
            raise ApiException(status=404,reason=f"Result output for job {workflowname} was not found. {resp}")

        if not resp:
            print(f"couldn't not find {mount_folder}/{workflowname}/*/{outputfile}")

        if not keepworkspace:
            # Calling exec and waiting for response
            print(f"Cleaning {workflowname} data from namespace {namespace}")
            exec_command = [
                '/bin/sh',
                '-c',
                f"rm -fr {mount_folder}/output-data/{workflowname} {mount_folder}/input-data/{workflowname} {mount_folder}/tmpout/{workflowname}  >&2"]
            cleanresp = stream(api_instance.connect_get_namespaced_pod_exec,
                        name,
                        namespace,
                        command=exec_command,
                        stderr=True, stdin=False,
                        stdout=True, tty=False)

            if "rm: can't remove" in cleanresp:
                raise ApiException(status=404,reason=f"Cannot clean job {workflowname}. {cleanresp}")
                

        return eval(resp)
    finally:
        resp = api_instance.delete_namespaced_pod(name=name, namespace=namespace)
