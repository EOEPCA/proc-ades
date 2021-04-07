from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint



"""
Get resource manager workspace details
"""

def read_getworkspace(workspace_id, response: Response):
    print("calling resource manager api")

    # Configure API key authorization: ApiKeyAuth
    configuration = rm_client.Configuration()
    configuration.host = "https://workspace-api.185.52.193.87.nip.io"
    # create an instance of the API class
    api_instance = rm_client.DefaultApi(rm_client.ApiClient(configuration))


    try:
        # Get Workspace
        api_response = api_instance.get_workspace_workspaces_workspace_name_get(workspace_id)
        pprint(api_response)
    except ApiException as e:
        print("Exception when calling DefaultApi->get_workspace_workspaces_workspace_name_get: %s\n" % e)

    return api_response