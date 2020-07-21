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





def process_inputs(cwl_document, job_input_json_file, volume_name_prefix, outputFolder, namespace):
    print(job_input_json_file, file=sys.stderr)
    job_input_json = json.load(open(job_input_json_file))

    print("parsing cwl", file=sys.stderr)
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

        # if input is of type stac:collection then a stage-in is required
        if "stac:collection" in v:

            type = v["type"].replace("[]", "")
            print(f"Input {k} is of type {v['type']} and contains a stac:collection. Stac-stage-in will run ", file=sys.stderr)

            input_counter = 1
            stagein_params = {}
            for input in job_input_json["inputs"]:
                if input['id'] == k:
                    stac_catalog_yaml = {"catalog": {}}
                    stac_catalog_yaml["catalog"]["id"] = "catid"
                    stac_catalog_yaml["catalog"]["title"] = "cat title"
                    stac_catalog_yaml["catalog"]["description"] = "cat description"
                    stac_catalog_yaml["catalog"]["collections"] = []
                    stac_catalog_yaml["catalog"]["collections"]
                    stac_catalog_yaml_entry = {k: {}}
                    stac_catalog_yaml_entry[k]["title"] = v["label"]
                    stac_catalog_yaml_entry[k]["description"] = v["label"] + " catalogue reference"
                    stac_catalog_yaml_entry[k]["entries"] = [input['value']]
                    stac_catalog_yaml["catalog"]["collections"].append(stac_catalog_yaml_entry)

                    stagein_params["volume_name_prefix"] = volume_name_prefix
                    stagein_params["mountFolder"] = outputFolder
                    stagein_params["outputFolder"] = path.join(outputFolder, f"input{str(input_counter)}")
                    stagein_params["namespace"] = namespace
                    stagein_params["timeout"] = 30000

                    print("Running stac stage-in", file=sys.stderr)

                    temp = tempfile.NamedTemporaryFile(mode='w+t', suffix=".yaml")


                    yamlString = yaml.dump(stac_catalog_yaml)

                    temp.write(yamlString)
                    temp.seek(0)
                    print(temp.name, file=sys.stderr)
                    print(temp.read(), file=sys.stderr)
                    stagein_params["input_stac_catalog"] = temp.name
                    os.chmod(temp.name, 0o777)
                    stagein.stac_stagein_run(SimpleNamespace(**stagein_params))
                    temp.close()

                    if k not in inputs:
                        inputs[k] = []
                    inputs[k].append({"class": type, "path": stagein_params["outputFolder"]})
                    input_counter += 1


        else:
            inputs[k] = {}
            for input in job_input_json["inputs"]:
                if input['id'] == k:
                    inputs[k] = input['value']
            break

    pprint(inputs, sys.stderr)
    return inputs


def run(args):
    namespace = args.namespace
    volume_name_prefix = args.volume_name_prefix
    mount_folder = args.mount_folder

    # volumes
    input_volume_name = volume_name_prefix + "-input-data"
    output_volume_name = volume_name_prefix + "-output-data"
    tmpout_volume_name = volume_name_prefix + "-tmpout"

    # workflow arguments
    cwl_document = args.cwl_file
    workflow_id = helpers.getCwlWorkflowId(cwl_document)
    workflow_name = args.workflowname

    package_directory = path.dirname(path.abspath(__file__))
    job_input_json = args.input_json  # json.loads(path.join(package_directory, 'examples' , 'job_order_old.json'))
    cwl_input_json = process_inputs(cwl_document, job_input_json, volume_name_prefix,
                                    path.join(mount_folder, "input-data"), namespace)

    # copying cwl in volume -input-data
    targetFolder = path.join(mount_folder, "input-data")
    print(f"Uploading cwl and input json to {targetFolder}", file=sys.stderr)

    tmppath = "/tmp/inputs.json"
    f = open("/tmp/inputs.json", "w")
    f.write(json.dumps(cwl_input_json))
    f.close()
    helpers.copy_files_to_volume(sources=[cwl_document, tmppath], targetFolder=mount_folder, mountFolder=mount_folder,
                                 persistentVolumeClaimName=input_volume_name, namespace=namespace)

    os.remove(tmppath)

    jsonInputFilename = ntpath.basename(tmppath)
    cwlDocumentFilename = ntpath.basename(cwl_document)

    # # Setup K8 configs
    configuration = client.Configuration()
    api_instance = client.BatchV1Api(client.ApiClient(configuration))
    yamlFileTemplate = "CalrissianJobTemplate.yaml"

    with open(path.join(path.dirname(__file__), yamlFileTemplate)) as f:

        print(f"Customizing stage-in job using the template {yamlFileTemplate} ", file=sys.stderr)
        # volume
        yaml_modified = f.read().replace("name: calrissianjob", f"name: {workflow_name}")
        yaml_modified = yaml_modified.replace("/calrissian/output-data", path.join(mount_folder, "output-data"))
        yaml_modified = yaml_modified.replace("/calrissian/input-data", path.join(mount_folder, "input-data"))
        yaml_modified = yaml_modified.replace("/calrissian/tmpout", path.join(mount_folder, "tmpout"))
        yaml_modified = yaml_modified.replace("revsort-array-job.json", jsonInputFilename)
        yaml_modified = yaml_modified.replace("revsort-array.cwl", f"{cwlDocumentFilename}#{workflow_id}")
        yaml_modified = yaml_modified.replace("revsort", workflow_id)
        yaml_modified = yaml_modified.replace("calrissian-", f"{volume_name_prefix}-")

        body = yaml.safe_load(yaml_modified)
        pprint(body, sys.stderr)

        try:
            resp = api_instance.create_namespaced_job(body=body, namespace=namespace)
            print("Job created. status='%s'" % str(resp.status), file=sys.stderr)
        except ApiException as e:
            print("Exception when submitting job: %s\n" % e, file=sys.stderr)
            return 1
