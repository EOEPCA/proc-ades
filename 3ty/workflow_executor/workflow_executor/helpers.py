import os
import sys
from pprint import pprint

import yaml
from kubernetes import client, config
from kubernetes.client import Configuration
from kubernetes.client.rest import ApiException


def get_api_client():
    proxy_url = os.getenv('HTTP_PROXY', None)

    if proxy_url:
        print("Setting proxy: {}".format(proxy_url))
        my_api_config = Configuration()
        my_api_config.host = proxy_url
        my_api_config.proxy = proxy_url
        my_api_config = client.ApiClient(my_api_config)
    else:
        config.load_kube_config()
        my_api_config = client.ApiClient()

    return my_api_config


def create_configmap_object(source, namespace, configmap_name,dataname):
    configmap_dict = dict()
    with open(source) as f:
        source_content = f.read()
    configmap_dict[dataname] = source_content
    # Configurate ConfigMap metadata
    metadata = client.V1ObjectMeta(
        deletion_grace_period_seconds=30,
        name=configmap_name,
        namespace=namespace,
    )
    # Instantiate the configmap object
    configmap = client.V1ConfigMap(
        api_version="v1",
        kind="ConfigMap",
        data=configmap_dict,
        metadata=metadata
    )
    return configmap


def create_configmap(source, namespace, configmap_name,dataname):
    configmap = create_configmap_object(source=source, namespace=namespace, configmap_name=configmap_name,dataname=dataname)
    api_client = get_api_client()
    api_instance = client.CoreV1Api(api_client)
    print("Creating configmap")
    try:
        api_response = api_instance.create_namespaced_config_map(
            namespace=namespace,
            body=configmap,
            pretty=True,
        )
        pprint(api_response)

    except ApiException as e:
        print("Exception when calling CoreV1Api->create_namespaced_config_map: %s\n" % e)
        raise e


def getCwlWorkflowId(cwl_path):
    print("parsing cwl to retrieve the Workflow Id")
    with open(cwl_path, 'r') as stream:
        try:
            graph = yaml.load(stream, Loader=yaml.FullLoader)["$graph"]
        except yaml.YAMLError as exc:
            print(exc, file=sys.stderr)

    for item in graph:
        if item.get('class') == "Workflow":
            return item.get('id')


def getCwlResourceRequirement(cwl_content):
    print("parsing cwl to retrieve the ResourceRequirements")
    try:
        graph = yaml.load(cwl_content, Loader=yaml.FullLoader)["$graph"]
    except yaml.YAMLError as exc:
        print(exc, file=sys.stderr)

    for item in graph:
        if item.get('class') == "CommandLineTool":
            try:
                return item["requirements"]["ResourceRequirement"]
            except KeyError:
                return None
