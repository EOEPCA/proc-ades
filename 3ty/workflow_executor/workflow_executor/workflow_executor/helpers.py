import os

import rm_client
import sys
from pprint import pprint

import yaml
from kubernetes import client, config
from kubernetes.client import Configuration
from kubernetes.client.rest import ApiException

from workflow_executor import eoepcaclient
import json

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


def create_configmap_object(source, namespace, configmap_name, dataname):
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


def create_configmap(source, namespace, configmap_name, dataname):
    configmap = create_configmap_object(source=source, namespace=namespace, configmap_name=configmap_name,
                                        dataname=dataname)
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


def retrieveLogs(controllerUid, namespace):
    # create an instance of the API class
    apiclient = get_api_client()
    api_instance = client.BatchV1Api(api_client=apiclient)
    core_v1 = client.CoreV1Api(api_client=apiclient)

    # controllerUid = api_response.metadata.labels["controller-uid"]
    pod_label_selector = "controller-uid=" + controllerUid
    pods_list = core_v1.list_namespaced_pod(namespace=namespace, label_selector=pod_label_selector, timeout_seconds=10)
    pod_name = pods_list.items[0].metadata.name
    try:
        # For whatever reason the response returns only the first few characters unless
        # the call is for `_return_http_data_only=True, _preload_content=False`
        pod_log_response = core_v1.read_namespaced_pod_log(name=pod_name, namespace=namespace,
                                                           _return_http_data_only=True, _preload_content=False)
        pod_log = pod_log_response.data.decode("utf-8")
        return pod_log

    except client.rest.ApiException as e:
        print("Exception when calling CoreV1Api->read_namespaced_pod_log: %s\n" % e)
        raise e


def storeLogs(logs, path):
    f = open(path, "a")
    f.write(logs)
    f.close()


def getResourceManagerWorkspaceDetails(resource_manager_endpoint,platform_domain, workspace_name, user_id_token= None):
    print("getResourceManagerWorkspaceDetails start")

    print("Registering client")
    demo = eoepcaclient.DemoClient(platform_domain)
    demo.register_client()
    demo.save_state()
    print("Client succesfully registered")


    print("Calling workspace api")
    workspace_access_token = None
    response, workspace_access_token = demo.workspace_get_details(resource_manager_endpoint, workspace_name,
                                                                  id_token=user_id_token,
                                                                  access_token=workspace_access_token)
    workspace_details = response.json()
    print(json.dumps(workspace_details, indent=2))

    print("getResourceManagerWorkspaceDetails end")
    return workspace_details


def registerResourceManagerWorkspace(resource_manager_endpoint, workspace_id, s3PAthUrl):
    if not resource_manager_endpoint:
        raise ApiException(reason="Resource manager endpoint could not be found")

    # Configure API key authorization: ApiKeyAuth
    configuration = rm_client.Configuration()
    configuration.host = resource_manager_endpoint
    # create an instance of the API class
    api_instance = rm_client.DefaultApi(rm_client.ApiClient(configuration))
    body = rm_client.Product(type="stac-item", url=s3PAthUrl)  # Product |

    try:
        # Register
        api_response = api_instance.register_workspaces_workspace_name_register_post(body, workspace_id)
        pprint(api_response)
    except rm_client.rest.ApiException as e:
        print("Exception when calling DefaultApi->register_workspaces_workspace_name_register_post: %s\n" % e)
        raise e

    return api_response
