from fastapi import FastAPI, Response, status
from pydantic import BaseModel

# from pydantic import BaseModel

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



@app.get("/")
def read_root():
    return {"Hello": "World"}


@app.post("/prepare", status_code=status.HTTP_201_CREATED)
def read_prepare(content: PrepareContent):
    print('\n'*2)
    print("entry: /prepare")
    print(str(content))
    print('return: {"prepareID": "0303598c-d75f-11ea-9904-c4b301bbaa1f"}')
    return {"prepareID": "0303598c-d75f-11ea-9904-c4b301bbaa1f"}


@app.get("/prepare/{prepare_id}", status_code=status.HTTP_200_OK)
def read_read(prepare_id: str):
    print('\n'*2)
    print("entry: /prepare/{prepare_id}")
    print(prepare_id)
    return {}
    # 200 done
    # 100 ripassa dopo
    # 500 error


@app.post("/execute", status_code=status.HTTP_201_CREATED)
def read_execute(content: ExecuteContent):
    print('\n'*2)
    print("entry: /execute")
    print(str(content))
    print('return: {"jobID": "141b9d92-d75f-11ea-9c9a-27f34c7e8856"}')
    return {"jobID": "141b9d92-d75f-11ea-9c9a-27f34c7e8856"}


@app.get("/status/{service_id}/{run_id}/{prepare_id}/{job_id}", status_code=status.HTTP_200_OK)
def read_getstatus(service_id: str, run_id: str, prepare_id: str, job_id: str):
    print('\n'*2)
    print("entry: status/{service_id}/{run_id}/{prepare_id}/{job_id}")
    print(f'{service_id} {run_id} {prepare_id} {job_id}')
    print("percent: 100, msg: done")    
    
    return {"percent": 100, "msg": "done"}


@app.get("/result/{service_id}/{run_id}/{prepare_id}/{job_id}", status_code=status.HTTP_200_OK)
def read_getresult(service_id: str, run_id: str, prepare_id: str, job_id: str):
    print('\n'*2)
    print('entry: /result/{service_id}/{run_id}/{prepare_id}/{job_id}')
    print(f'{service_id} {run_id} {prepare_id} {job_id}')
    print('return: {"wf_output": {"catalog":"http:catalog.terradue.com/........."}}')
    return {"wf_output": '{"catalog":"http:catalog.terradue.com/........."}'}


@app.get("/testerror")
def test_error(response: Response):
    e = Error()
    e.set_error(12, "all the candies are gone!")
    response.status_code = status.HTTP_500_INTERNAL_SERVER_ERROR
    return e
