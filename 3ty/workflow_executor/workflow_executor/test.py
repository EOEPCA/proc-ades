import json
import os
import yaml
from pprint import pprint
from kubernetes import client, config, watch
from workflow_executor import helpers
import tempfile

stac_catalog_yaml = {}
stac_catalog_yaml["catalog"]={}
stac_catalog_yaml["catalog"]["id"]="cat"
stac_catalog_yaml["catalog"]["title"]="cat title"
stac_catalog_yaml["catalog"]["description"]="cat description"
stac_catalog_yaml["catalog"]["collections"] = []
stac_catalog_yaml["catalog"]["collections"].append("hello")

documents = yaml.dump(stac_catalog_yaml)
print(documents)
exit(0)


config.load_kube_config()
configuration = client.Configuration()
api_instance = client.CoreV1Api(client.ApiClient(configuration))

job_input_json_file= "/examples/job_order_old.json"
job_input_json = json.load(open(job_input_json_file))

f = open("/tmp/inputs.json", "w")
f.write(json.dumps(job_input_json))
f.close()



exit(0)

config.load_kube_config(
    os.path.join(os.environ["HOME"], '/var/snap/microk8s/current/credentials/kubelet.config'))

#helpers.mkdir_in_volume("/tmp/application/input-data/", "/tmp/application/input-data/","t2demo")

helpers.copy_files_to_volume(["/home/bla/dev/EOEPCA_dev/cwl-executor/workflow_executor/conda_build_config.yaml"],"/t2application/input-data/", "t2demo-volume-input-data","t2demo")

exit(0)
# config.load_kube_config(
#     os.path.join(os.environ["HOME"], '/var/snap/microk8s/current/credentials/kubelet.config'))
#
# namespace="t2demo"
# job_name="stage-in-d518a255-4751-4f8f-adbd-bacec40bf148"
#
#
# configuration = client.Configuration()
# api_instance_batch_v1_api = client.BatchV1Api(client.ApiClient(configuration))
# job_status_response = api_instance_batch_v1_api.read_namespaced_job_status(job_name, namespace, pretty=True)
# pprint(job_status_response)
#
# exit(0)
#
#
# api_instance_core_v1_api = client.CoreV1Api()
# podlist = api_instance_core_v1_api.list_namespaced_pod(namespace=namespace,label_selector=f'job-name={job_name}',watch=False)
# for pod in podlist.items:
#     pprint(pod)
#     print("%s\t%s\t%s" % (pod.metadata.name,
#                           pod.status.phase,
#                           pod.status.pod_ip))
#
#     pprint("####################")
#     if not pod.status.phase == "Pending":
#         api_response = api_instance_core_v1_api.read_namespaced_pod_log(name=pod.metadata.name, namespace=namespace)
#         print(api_response)
#
#
# exit(0)


cwl_document = "/home/bla/dev/EOEPCA_dev/cwl-executor/workflow_executor/examples/s3-slstr-tiling.cwl"
# parsing cwl
with open(cwl_document, 'r') as stream:
    try:
        graph = yaml.load(stream, Loader=yaml.FullLoader)["$graph"]
    except yaml.YAMLError as exc:
        print(exc)

for item in graph:
    if item.get('class') == "CommandLineTool":
        commandLineTool = item
    else:
        workflow = item

# print(f"commandLineTool..... {commandLineTool}")
# print(f"workflow..... {workflow}")

print(f"workflow inputs..... {workflow['inputs']}")
inputs = {}
for k, v in workflow['inputs'].items():
    inputs[k] = {}
    type = v["type"].replace("[]", "")
    inputs[k]["class"] = type

    if "stac:collection" in v:
        print(f"Input {k} is of type {v['type']} and contains a stac:collection. Stac-stage-in will run ")

    if type == "File" or type == "Directory":
        inputs[k]["path"] = "blabla"

pprint(inputs)
