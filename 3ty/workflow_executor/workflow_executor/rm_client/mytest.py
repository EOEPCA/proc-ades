import rm_client
from pprint import pprint

resource_manager_endpoint = "https://workspace-api.185.52.193.87.nip.io"
workspace_id = "rm-user-ericq"
configuration = rm_client.Configuration()
configuration.host = resource_manager_endpoint



# create an instance of the API class
api_instance = rm_client.DefaultApi(rm_client.ApiClient(configuration))



try:
    # Get Workspace
    #api_response = api_instance.get_workspace_workspaces_workspace_name_get(workspace_id)


    api_response, status_code, headers  = api_instance.get_workspace_workspaces_workspace_name_get_with_http_info(workspace_id)
    print(status_code)
    pprint(headers)



except rm_client.rest.ApiException as e:

    if e.status == 401:
        print("401 caught")

    if e.status == 404:
        print("404 caught")

    #print("Exception when calling DefaultApi->get_workspace_workspaces_workspace_name_get: %s\n" % e)
    #raise e


#pprint(api_response)