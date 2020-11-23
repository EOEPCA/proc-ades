from fastapi import FastAPI, Response, status, Header
from pydantic import BaseModel
from typing import Optional

app = FastAPI(
    title="the title",
    description="the config",
    version="2.5.0",
    openapi_url="/api",
    docs_url="/api/docs", redoc_url="/api/redoc"
)

class adesRequest(BaseModel):
    icon_uri: str
    name: str
    scopes: list


@app.get("/")
def read_root():
    return {"mock": "pep"}


@app.get("/resources")
def read_root(authorization: Optional[str] = Header(None)):
    print('\n'*2)
    print("entry: GET /resources")
    print(str(authorization))

    return [
        {
            "ownership_id": "d290f1ee-6c54-4b01-90e6-288571188183",
            "id": "d290f1ee-6c54-4b01-90e6-d701748f0851",
            "name": '/rdirienzo/watchjob/processes/vegetation_index_/jobs/717470c8-2a7e-11eb-b739-0242ac150004',
            "icon_uri": '/rdirienzo/wps3/processes/vegetation_index_',
            "scopes": ['public']
        },
        {
            "ownership_id": "d290f1ee-6c54-4b01-90e6-288571188183",
            "id": "d290f1ee-6c54-4b01-90e6-d701748f0851",
            "name": '/rdirienzo/watchjob/processes/vegetation_index_/jobs/717470c8-2a7e-11eb-b739-0242ac150004',
            "icon_uri": '/rdirienzo/wps3/processes/testC',
            "scopes": ['public']
        }
    ]

@app.post("/resources")
def read_root(content: adesRequest,authorization: Optional[str] = Header(None)):
    print('\n'*2)
    print("entry: POST /resources")
    print(str(content))
    print(str(authorization))

    return {
        "ownership_id": "d290f1ee-6c54-4b01-90e6-288571188183",
        "id": "d290f1ee-6c54-4b01-90e6-d701748f0851",
        "name": content.name,
        "icon_uri": content.icon_uri,
        "scopes": content.scopes
    }

@app.get("/resources/{resource_id}")
def read_root(prepare_id: str):
    return {"mock": f'get: /resources/{prepare_id}'}


@app.put("/resources/{resource_id}")
def read_root(prepare_id: str):
    return {"mock": f'put: /resources/{prepare_id}'}


@app.delete("/resources/{resource_id}")
def read_root(resource_id: str,authorization: Optional[str] = Header(None)):
    print('\n'*2)
    print("entry: delete /resources")
    print(str(authorization))
    print(resource_id)

    c= f"delete: /resources/{resource_id}"

    return {}

