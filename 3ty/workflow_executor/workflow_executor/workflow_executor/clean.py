import json
import time
from os import path
from pprint import pprint

from kubernetes import client, config
from kubernetes.client.rest import ApiException
from kubernetes.stream import stream
import sys

from workflow_executor import helpers


def run(namespace):
    try:

        # create an instance of the API class
        api_client = helpers.get_api_client()
        api_instance = client.CoreV1Api(api_client)
        resp = api_instance.delete_namespace(name=namespace,pretty=True)
        return resp

    except ApiException as e:
        print("Exception when calling CoreV1Api->delete_namespace: %s\n" % e)
        raise e