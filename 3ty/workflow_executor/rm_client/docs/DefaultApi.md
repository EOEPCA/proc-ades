# rm_client.DefaultApi

All URIs are relative to */*

Method | HTTP request | Description
------------- | ------------- | -------------
[**create_workspace_workspaces_post**](DefaultApi.md#create_workspace_workspaces_post) | **POST** /workspaces | Create Workspace
[**delete_workspace_workspaces_workspace_name_delete**](DefaultApi.md#delete_workspace_workspaces_workspace_name_delete) | **DELETE** /workspaces/{workspace_name} | Delete Workspace
[**get_workspace_workspaces_workspace_name_get**](DefaultApi.md#get_workspace_workspaces_workspace_name_get) | **GET** /workspaces/{workspace_name} | Get Workspace
[**patch_workspace_workspaces_workspace_name_patch**](DefaultApi.md#patch_workspace_workspaces_workspace_name_patch) | **PATCH** /workspaces/{workspace_name} | Patch Workspace
[**probe_probe_get**](DefaultApi.md#probe_probe_get) | **GET** /probe | Probe
[**register_workspaces_workspace_name_register_post**](DefaultApi.md#register_workspaces_workspace_name_register_post) | **POST** /workspaces/{workspace_name}/register | Register

# **create_workspace_workspaces_post**
> Object create_workspace_workspaces_post(body)

Create Workspace

### Example
```python
from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = rm_client.DefaultApi()
body = rm_client.WorkspaceCreate() # WorkspaceCreate | 

try:
    # Create Workspace
    api_response = api_instance.create_workspace_workspaces_post(body)
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->create_workspace_workspaces_post: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **body** | [**WorkspaceCreate**](WorkspaceCreate.md)|  | 

### Return type

[**Object**](Object.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **delete_workspace_workspaces_workspace_name_delete**
> delete_workspace_workspaces_workspace_name_delete(workspace_name)

Delete Workspace

### Example
```python
from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = rm_client.DefaultApi()
workspace_name = 'workspace_name_example' # str | 

try:
    # Delete Workspace
    api_instance.delete_workspace_workspaces_workspace_name_delete(workspace_name)
except ApiException as e:
    print("Exception when calling DefaultApi->delete_workspace_workspaces_workspace_name_delete: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **workspace_name** | **str**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **get_workspace_workspaces_workspace_name_get**
> Workspace get_workspace_workspaces_workspace_name_get(workspace_name)

Get Workspace

### Example
```python
from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = rm_client.DefaultApi()
workspace_name = 'workspace_name_example' # str | 

try:
    # Get Workspace
    api_response = api_instance.get_workspace_workspaces_workspace_name_get(workspace_name)
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->get_workspace_workspaces_workspace_name_get: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **workspace_name** | **str**|  | 

### Return type

[**Workspace**](Workspace.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **patch_workspace_workspaces_workspace_name_patch**
> patch_workspace_workspaces_workspace_name_patch(body, workspace_name)

Patch Workspace

### Example
```python
from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = rm_client.DefaultApi()
body = rm_client.WorkspaceUpdate() # WorkspaceUpdate | 
workspace_name = 'workspace_name_example' # str | 

try:
    # Patch Workspace
    api_instance.patch_workspace_workspaces_workspace_name_patch(body, workspace_name)
except ApiException as e:
    print("Exception when calling DefaultApi->patch_workspace_workspaces_workspace_name_patch: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **body** | [**WorkspaceUpdate**](WorkspaceUpdate.md)|  | 
 **workspace_name** | **str**|  | 

### Return type

void (empty response body)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **probe_probe_get**
> Object probe_probe_get()

Probe

### Example
```python
from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = rm_client.DefaultApi()

try:
    # Probe
    api_response = api_instance.probe_probe_get()
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->probe_probe_get: %s\n" % e)
```

### Parameters
This endpoint does not need any parameter.

### Return type

[**Object**](Object.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **register_workspaces_workspace_name_register_post**
> Object register_workspaces_workspace_name_register_post(body, workspace_name)

Register

### Example
```python
from __future__ import print_function
import time
import rm_client
from rm_client.rest import ApiException
from pprint import pprint

# create an instance of the API class
api_instance = rm_client.DefaultApi()
body = rm_client.Product() # Product | 
workspace_name = 'workspace_name_example' # str | 

try:
    # Register
    api_response = api_instance.register_workspaces_workspace_name_register_post(body, workspace_name)
    pprint(api_response)
except ApiException as e:
    print("Exception when calling DefaultApi->register_workspaces_workspace_name_register_post: %s\n" % e)
```

### Parameters

Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
 **body** | [**Product**](Product.md)|  | 
 **workspace_name** | **str**|  | 

### Return type

[**Object**](Object.md)

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

