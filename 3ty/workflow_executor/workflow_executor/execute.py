import json
import ntpath
import os
import sys
from types import SimpleNamespace
import yaml
from jinja2 import Template
from workflow_executor import helpers
from pprint import pprint
from os import path
from kubernetes import client, config
from kubernetes.client.rest import ApiException
import tempfile
from cwl_wrapper.parser import Parser
from io import StringIO


def process_inputs(cwl_document, job_input_json_file):
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
                    # inputs[k].append(input['input']['value'])
                    inputs[k].append(input['value'])
                else:
                    inputs[k] = {}
                    # inputs[k] = input['input']['value']
                    inputs[k] = input['value']

    print("Input json to pass to the cwl runner: ")
    pprint(inputs)
    return inputs


def run(namespace, volume_name_prefix, mount_folder, cwl_document, job_input_json, workflow_name,
        max_ram="4G", max_cores="2", cwl_wrapper_config=None, state=None):
    # volumes
    input_volume_name = volume_name_prefix + "-input-data"
    output_volume_name = volume_name_prefix + "-output-data"
    tmpout_volume_name = volume_name_prefix + "-tmpout"

    # cwl-wrapper
    wrapped_cwl_document = wrapcwl(cwl_document, cwl_wrapper_config)

    # remove std.out and std.err lines to let calrissian take care of it
    delete_line_by_full_match(wrapped_cwl_document, "  stderr: std.err")
    delete_line_by_full_match(wrapped_cwl_document, "  stdout: std.out")

    workflow_id = helpers.getCwlWorkflowId(cwl_document)
    package_directory = path.dirname(path.abspath(__file__))
    cwl_input_json = process_inputs(wrapped_cwl_document, job_input_json)
    cwl_input_json["workflow"]=workflow_id
    cwl_input_json["process"]=workflow_name

    # copying cwl in volume -input-data
    targetFolder = path.join(mount_folder, "input-data")
    print(f"Uploading cwl and input json to {targetFolder}")

    tmppath = "/tmp/inputs.json"
    f = open("/tmp/inputs.json", "w")
    f.write(json.dumps(cwl_input_json))
    f.close()

    cwl_config = "cwl-config"
    inputs_config = "inputs-config"
    helpers.create_configmap(source=wrapped_cwl_document, namespace=namespace, configmap_name=cwl_config,
                             dataname="cwl")
    helpers.create_configmap(source=tmppath, namespace=namespace, configmap_name=inputs_config, dataname="inputs")

    os.remove(tmppath)

    jsonInputFilename = ntpath.basename(tmppath)
    cwlDocumentFilename = ntpath.basename(wrapped_cwl_document)

    # # Setup K8 configs
    apiclient = helpers.get_api_client()
    api_instance = client.BatchV1Api(apiclient)

    yamlFileTemplate = "CalrissianJobTemplate.yaml"

    with open(path.join(path.dirname(__file__), yamlFileTemplate)) as f:

        print(f"Customizing stage-in job using the template {yamlFileTemplate} ")

        template = Template(f.read())
        variables = {"jobname": workflow_name,
                     "stdout": path.join(mount_folder, "output-data", workflow_name, f"{workflow_id}-output.json"),
                     "stderr": path.join(mount_folder, "output-data", workflow_name, f"{workflow_id}-stderr.log"),
                     "max_ram": max_ram,
                     "max_cores": max_cores,
                     "tmp_outdir_prefix": f"{path.join(mount_folder, 'tmpout', workflow_name)}/",
                     "tmpdir_prefix": f"{path.join(mount_folder, 'tmpout', workflow_name)}/",
                     "outdir": f"{path.join(mount_folder, 'output-data', workflow_name)}/",
                     "argument1": path.join(mount_folder, "input-data", workflow_name,
                                            f"{cwlDocumentFilename}#{workflow_id}"),
                     "argument2": path.join(mount_folder, "input-data", workflow_name, jsonInputFilename),
                     "cwl_file_path": path.join(mount_folder, "input-data", workflow_name, f"{cwlDocumentFilename}"),
                     "inputs_file_path": path.join(mount_folder, "input-data", workflow_name, jsonInputFilename),
                     "volumemount_tmpout_mount_path": path.join(mount_folder, "tmpout"),
                     "volumemount_tmpout_name": tmpout_volume_name,
                     "volumemount_output_data_mount_path": path.join(mount_folder, "output-data"),
                     "volumemount_output_data_name": output_volume_name}

        yaml_modified = template.render(variables)

        body = yaml.safe_load(yaml_modified)
        pprint(body)

        try:
            resp = api_instance.create_namespaced_job(body=body, namespace=namespace)
            print("Job created. status='%s'" % str(resp.status))
        except ApiException as e:
            print("Exception when submitting job: %s\n" % e, file=sys.stderr)
            return e


def wrapcwl(cwl_document, cwl_wrapper_config=None):
    directory = os.path.dirname(cwl_document)

    filename = os.path.basename(cwl_document)
    filename_wo_extension = os.path.splitext(filename)[0]

    # default cwl_wrapper_configs
    wrappedcwl = os.path.join(directory, f"{filename_wo_extension}_wrapped.cwl")

    if cwl_wrapper_config:
        k = dict()
        k['cwl'] = cwl_document
        k['rulez'] = cwl_wrapper_config['rulez'] if cwl_wrapper_config['rulez'] else None
        k['output'] = wrappedcwl
        k['maincwl'] = cwl_wrapper_config['maincwl'] if cwl_wrapper_config['maincwl'] else None
        k['stagein'] = cwl_wrapper_config['stagein'] if cwl_wrapper_config['stagein'] else None
        k['stageout'] = cwl_wrapper_config['stageout'] if cwl_wrapper_config['stageout'] else None
        k['assets'] = None
    else:
        k = dict()
        k['cwl'] = cwl_document
        k['rulez'] = None
        k['output'] = wrappedcwl
        k['maincwl'] = None
        k['stagein'] = None
        k['stageout'] = None
        k['assets'] = None

    wf = Parser(k)
    wf.write_output()

    with open(wrappedcwl, 'r') as f:
        print("# WRAPPED CWL")
        print(f.read())
        print("# END WRAPPED CWL")

    return wrappedcwl


def delete_line_by_full_match(original_file, line_to_delete):
    """ In a file, delete the lines at line number in given list"""
    is_skipped = False
    dummy_file = original_file + '.bak'
    # Open original file in read only mode and dummy file in write mode
    with open(original_file, 'r') as read_obj, open(dummy_file, 'w') as write_obj:
        # Line by line copy data from original file to dummy file
        for line in read_obj:
            line_to_match = line
            if line[-1] == '\n':
                line_to_match = line[:-1]
            # if current line matches with the given line then skip that line
            if line_to_match != line_to_delete:
                write_obj.write(line)
            else:
                is_skipped = True
    # If any line is skipped then rename dummy file as original file
    if is_skipped:
        os.remove(original_file)
        os.rename(dummy_file, original_file)
    else:
        os.remove(dummy_file)
