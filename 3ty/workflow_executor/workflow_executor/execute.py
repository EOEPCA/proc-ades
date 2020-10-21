import json
import ntpath
import os
import sys
from types import SimpleNamespace
import yaml
from workflow_executor import helpers, stagein
from pprint import pprint
from os import path
from kubernetes import client, config
from kubernetes.client.rest import ApiException
import tempfile
from cwl_wrapper.parser import Parser
from io import StringIO


def process_inputs(cwl_document, job_input_json_file, volume_name_prefix, outputFolder, namespace, state=None):
    print(job_input_json_file)
    job_input_json = json.load(open(job_input_json_file))

    
    print("parsing cwl")
    with open(cwl_document, 'r') as stream:
        try:
            graph = yaml.load(stream, Loader=yaml.FullLoader)["$graph"]
        except yaml.YAMLError as exc:
            print(exc)

    for item in graph:
        if item.get('class') == "Workflow":
            workflow = item
            break

    # iterating through list of inputs of cwl
    inputs = {}
    for k, v in workflow['inputs'].items():


        for input in job_input_json["inputs"]:

            if input['id'] == k:
                type = v["type"]

                if "[]" in type:
                    if k not in inputs.keys():
                        inputs[k] = []
                    #inputs[k].append(input['input']['value'])
                    inputs[k].append(input['value'])
                else:
                    inputs[k] = {}
                    #inputs[k] = input['input']['value']
                    inputs[k] = input['value']

    print("Input json to pass to the cwl runner: ")
    pprint(inputs)
    return inputs


def run(namespace, volume_name_prefix, mount_folder, cwl_document, job_input_json, workflow_name, state=None):
    # volumes
    input_volume_name = volume_name_prefix + "-input-data"
    output_volume_name = volume_name_prefix + "-output-data"
    tmpout_volume_name = volume_name_prefix + "-tmpout"


    # cwl-wrapper
    wrapped_cwl_document=wrapcwl(cwl_document)

    workflow_id = helpers.getCwlWorkflowId(wrapped_cwl_document)
    package_directory = path.dirname(path.abspath(__file__))
    cwl_input_json = process_inputs(wrapped_cwl_document, job_input_json, volume_name_prefix,
                                    path.join(mount_folder, "input-data"), namespace, state=state)

    # copying cwl in volume -input-data
    targetFolder = path.join(mount_folder, "input-data")
    print(f"Uploading cwl and input json to {targetFolder}")

    tmppath = "/tmp/inputs.json"
    f = open("/tmp/inputs.json", "w")
    f.write(json.dumps(cwl_input_json))
    f.close()
    helpers.copy_files_to_volume(sources=[wrapped_cwl_document, tmppath], targetFolder=mount_folder, mountFolder=mount_folder,
                                 persistentVolumeClaimName=input_volume_name, namespace=namespace, state=state, workflow_name=workflow_name)

    os.remove(tmppath)

    jsonInputFilename = ntpath.basename(tmppath)
    cwlDocumentFilename = ntpath.basename(wrapped_cwl_document)

    # # Setup K8 configs
    config.load_kube_config()
    api_instance = client.BatchV1Api(client.ApiClient())
    yamlFileTemplate = "CalrissianJobTemplate.yaml"

    with open(path.join(path.dirname(__file__), yamlFileTemplate)) as f:

        print(f"Customizing stage-in job using the template {yamlFileTemplate} ")
        # volume
        yaml_modified = f.read().replace("name: calrissianjob", f"name: {workflow_name}")
        yaml_modified = yaml_modified.replace("/calrissian/output-data", path.join(mount_folder, "output-data"))
        yaml_modified = yaml_modified.replace("/calrissian/input-data", path.join(mount_folder, "input-data"))
        yaml_modified = yaml_modified.replace("/calrissian/tmpout", path.join(mount_folder, "tmpout"))
        yaml_modified = yaml_modified.replace("revsort-array-job.json", jsonInputFilename)
        yaml_modified = yaml_modified.replace("revsort-array.cwl", f"{cwlDocumentFilename}#{workflow_id}")
        yaml_modified = yaml_modified.replace("revsort", workflow_id)
        yaml_modified = yaml_modified.replace("calrissian-", f"{volume_name_prefix}-")
        yaml_modified = yaml_modified.replace("t2workflow123", f"{workflow_name}")
        yaml_modified = yaml_modified.replace("calrissian-output.json", f"{workflow_id}-output.json")
        yaml_modified = yaml_modified.replace("stac-output.out", f"{workflow_name}-stac-output.out")

        body = yaml.safe_load(yaml_modified)
        pprint(body)

        try:
            resp = api_instance.create_namespaced_job(body=body, namespace=namespace)
            print("Job created. status='%s'" % str(resp.status))
        except ApiException as e:
            print("Exception when submitting job: %s\n" % e, file=sys.stderr)
            return e



def wrapcwl(cwl_document):

    directory=os.path.dirname(cwl_document)

    filename=os.path.basename(cwl_document)
    filename_wo_extension=os.path.splitext(filename)[0]
    wrappedcwl=os.path.join(directory,f"{filename_wo_extension}_wrapped.cwl")

    k = dict()
    k['cwl'] = cwl_document
    k['rulez'] = None
    k['output'] = wrappedcwl
    k['maincwl'] = None
    k['stagein'] = None
    k['stageout'] = None
    wf = Parser(k)
    wf.write_output()
    return wrappedcwl