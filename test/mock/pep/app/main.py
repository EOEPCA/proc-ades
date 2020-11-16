from fastapi import FastAPI, Response, status
from pydantic import BaseModel


app = FastAPI(
    title="the title",
    description="the config",
    version="2.5.0",
    openapi_url="/api",
    docs_url="/api/docs", redoc_url="/api/redoc"
)


@app.get("/")
def read_root():
    return {"mock": "pep"}


@app.get("/resources")
def read_root():
    return {"mock": "/resources"}

@app.post("/resources")
def read_root():
    return {"mock": 'post: /resources'}


@app.get("/resources/{resource_id}")
def read_root(prepare_id: str):
    return {"mock": f'get: /resources/{prepare_id}'}


@app.put("/resources/{resource_id}")
def read_root(prepare_id: str):
    return {"mock": f'put: /resources/{prepare_id}'}


@app.delete("/resources/{resource_id}")
def read_root(prepare_id: str):
    return {"mock": f'delete: /resources/{prepare_id}'}
