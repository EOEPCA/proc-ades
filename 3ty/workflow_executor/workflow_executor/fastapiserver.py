import json
import os
import tempfile
import uvicorn
from fastapi import FastAPI, Form, File, status, Response
from fastapi.responses import JSONResponse
from fastapi.encoders import jsonable_encoder
import workflow_executor
from workflow_executor import prepare, client, result
from pydantic import BaseModel
from kubernetes.client.rest import ApiException
from pprint import pprint

app = FastAPI(
    title="the title",
    description="the config",
    version="2.5.0",
    openapi_url="/api",
    docs_url="/api/docs", redoc_url="/api/redoc"
)



class Error:

    def __init__(self):
        self.err = {
            "error": {
                "code": 0,
                "message": ""
            }
        }

    def set_error(self, code, msg):
        self.err["error"]["code"] = code
        self.err["error"]["message"] = msg

    def __str__(self):
        return self.err


class PrepareContent(BaseModel):
    serviceID: str
    runID: str


class ExecuteContent(PrepareContent):
    prepareID: str
    cwl: str
    inputs: str






def sanitize_k8_parameters(value: str):
    value = value.replace("_", "-").lower()
    while value.endswith("-"):
        value = value[:-1]
    return value

@app.get("/")
def read_root():
    return {"Hello": "World"}


@app.post("/prepare", status_code=status.HTTP_201_CREATED)
def read_prepare(content: PrepareContent, response: Response):
    state = client.State()
    print('Prepare POST')


    namespace = sanitize_k8_parameters(content.serviceID)
    volumeSize = 4
    volumeName = f"{namespace}-volume"

    print('namespace: %s' % namespace)
    print('volume_size: %d' % volumeSize)
    print('volume_name: %s' % volumeName)

    default_value = ""
    workflow_config = {
        "storageclass": os.getenv('STORAGE_CLASS', default_value),
        "volumesize": os.getenv('VOLUME_SIZE', volumeSize)
    }

    try:
        resp_status = workflow_executor.prepare.run(namespace=namespace, volumeSize=volumeSize, volumeName=volumeName,
                                                    state=state, workflow_config=workflow_config)
    except ApiException as e:
        response.status_code = e.status

    return {"prepareID": namespace}


@app.get("/prepare/{prepare_id}", status_code=status.HTTP_200_OK)
def read_prepare(prepare_id: str, response: Response):
    namespace = sanitize_k8_parameters(prepare_id)

    state = client.State()
    print('Prepare GET')


    try:
        resp_status = workflow_executor.prepare.get(namespace=namespace, state=state)
    except ApiException as e:
        response.status_code = e.status

    if resp_status["status"] == "pending":
        response.status_code = status.HTTP_100_CONTINUE

    return resp_status
    # 200 done
    # 100 ripassa dopo
    # 500 error


@app.post("/execute", status_code=status.HTTP_201_CREATED)
def read_execute(content: ExecuteContent, response: Response):
    # {"runID": "runID-123","serviceID": "service-id-123", "prepareID":"uuid" ,"cwl":".......","inputs":".........."}

    state = client.State()
    print('Execute POST')

    namespace = sanitize_k8_parameters(content.serviceID)
    cwl_content = content.cwl
    inputs_content = json.loads(content.inputs)
    volume_name_prefix = f"{namespace}-volume"
    workflow_name = sanitize_k8_parameters(f"wf-{content.runID}")
    mount_folder = "/workflow"


    # retrieve config params and store them in json
    # these will be used in the stageout phase
    default_value = ""

    inputs_content["inputs"].append({
        "id":"store_host",
        "dataType": "string",
        "value":os.getenv('STORAGE_HOST', default_value),
        "mimeType": "",
        "href": ""})

    inputs_content["inputs"].append({
        "id":"store_username",
        "dataType": "string",
        "value":os.getenv('STORAGE_USERNAME', default_value),
        "mimeType": "",
        "href": ""})

    inputs_content["inputs"].append({
        "id":"store_apikey",
        "dataType": "string",
        "value":os.getenv('STORAGE_APIKEY', default_value),
        "mimeType": "",
        "href": ""})

    inputs_content["inputs"].append({
        "id": "job",
        "dataType": "string",
        "value": workflow_name,
        "mimeType": "",
        "href": ""})

    inputs_content["inputs"].append({
        "id": "outputfile",
        "dataType": "string",
        "value": f"{workflow_name}.res",
        "mimeType": "",
        "href": ""})        

    pprint(f"inputs_content {inputs_content}")
    # inputcwlfile is input_json + cwl_file
    # create 2 temp files
    with tempfile.NamedTemporaryFile(mode="w") as cwl_file, tempfile.NamedTemporaryFile(mode="w") as input_json:
        cwl_file.write(cwl_content)
        cwl_file.flush()
        cwl_file.seek(0)

        input_json.write(json.dumps(inputs_content))
        input_json.flush()
        input_json.seek(0)

        print(cwl_file.name)
        print(input_json.name)

        try:
            resp_status = workflow_executor.execute.run(state=state,
                                                        cwl_document=cwl_file.name,
                                                        job_input_json=input_json.name,
                                                        volume_name_prefix=volume_name_prefix,
                                                        mount_folder=mount_folder,
                                                        namespace=namespace,
                                                        workflow_name=workflow_name)
        except ApiException as e:
            response.status_code = e.status
            resp_status = {"status": "failed", "error": e.body}

    return {"jobID": workflow_name }


@app.get("/status/{service_id}/{run_id}/{prepare_id}/{job_id}", status_code=status.HTTP_200_OK)
def read_getstatus(service_id: str, run_id: str, prepare_id: str, job_id: str, response: Response):
    namespace = sanitize_k8_parameters(service_id)
    workflow_name = sanitize_k8_parameters(f"wf-{run_id}")

    state = client.State()
    print('Status GET')

    resp_status = None
    from fastapi import status
    try:
        resp_status = workflow_executor.status.run(namespace=namespace, workflow_name=workflow_name, state=state)

        if resp_status["status"] == "Running":
            response.status_code = status.HTTP_200_OK
            status = {"percent": 50, "msg": "running"}
        elif resp_status["status"] == "Success":
            response.status_code = status.HTTP_200_OK
            status = {"percent": 100, "msg": "done"}
        elif resp_status["status"] == "Failed":
            e = Error()
            e.set_error(12, resp_status["error"])
            response.status_code = status.HTTP_500_INTERNAL_SERVER_ERROR
            return e


    except ApiException as err:
            e = Error()
            e.set_error(12, err.body)
            response.status_code = status.HTTP_500_INTERNAL_SERVER_ERROR
            return e

    return status


@app.get("/result/{service_id}/{run_id}/{prepare_id}/{job_id}", status_code=status.HTTP_200_OK)
def read_getresult(service_id: str, run_id: str, prepare_id: str, job_id: str, response: Response):
    namespace = sanitize_k8_parameters(service_id)
    workflow_name = sanitize_k8_parameters(f"wf-{run_id}")
    volume_name_prefix = f"{namespace}-volume"
    mount_folder = "/workflow"
    outputfile=f"{workflow_name}.res"

    state = client.State()
    print('Result GET')

    resp_status = {}
    try:
        resp_status = workflow_executor.result.run(namespace=namespace, workflowname=workflow_name,
                                                   mount_folder=mount_folder, volume_name_prefix=volume_name_prefix,outputfile=outputfile,
                                                   state=state)
        print("getresult success")
        pprint(resp_status)
    except ApiException as err:
            e = Error()
            e.set_error(12, err.body)
            print(err.body)
            response.status_code = status.HTTP_500_INTERNAL_SERVER_ERROR
            return e

    json_compatible_item_data = {'wf_output': json.dumps(resp_status)}
    pprint("wf_output json: ")
    pprint(json_compatible_item_data)
    return JSONResponse(content=json_compatible_item_data)


def main():
    print("DEBuG MODE")
    uvicorn.run(app)


if __name__ == "__main__":
    main()