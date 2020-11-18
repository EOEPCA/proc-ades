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
def read_root():
    return {"mock": "/resources"}

@app.post("/resources")
def read_root(content: adesRequest,authorization: Optional[str] = Header(None)):

    print('\n'*2)
    print("entry: /resources")
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
def read_root(prepare_id: str):
    return {"mock": f'delete: /resources/{prepare_id}'}
