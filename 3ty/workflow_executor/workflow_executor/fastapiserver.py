import tempfile
from types import SimpleNamespace
from typing import Optional

import uvicorn
from fastapi import FastAPI, Form, File

import workflow_executor
from workflow_executor import prepare, client


app = FastAPI()

kubeconfig = "/var/snap/microk8s/current/credentials/kubelet.config"

@app.get("/")
def read_root():
    return {"Hello": "World"}





@app.post("/prepare/")
async def prepareCall(namespace: str = Form(...),
                      volumeSize: int = Form(...),
                      volumeName: str = Form(...)):



    state = client.State()
    state.kubeconfig = kubeconfig
    print('Kube config: %s' % kubeconfig)
    print('Verbosity: %s' % state.verbosity)
    print('Debug: %s' % state.debug)

    print('namespace: %s' % namespace)
    print('volume_size: %d' % volumeSize)
    print('volume_name: %s' % volumeName)

    resp_status = workflow_executor.prepare.run(namespace=namespace, volumeSize=volumeSize, volumeName=volumeName,
                                                state=state)
    return resp_status





@app.post("/execute/")
async def executeCall(inputcwlfile: bytes = File(...),
                      namespace: str = Form(...),
                      volume_name_prefix: str = Form(...),
                      workflow_name: str = Form(...),
                      mount_folder: str = Form(...)):

    state = client.State()
    state.kubeconfig = kubeconfig
    print('Kube config: %s' % kubeconfig)
    print('Verbosity: %s' % state.verbosity)
    print('Debug: %s' % state.debug)

    # inputcwlfile is input_json + cwl_file
    # create 2 temp files
    with tempfile.NamedTemporaryFile() as cwl_file, tempfile.NamedTemporaryFile() as input_json:

        # parse the inputcwlfile json and divide
        #inputcwlfile

        cwl_file.write('Some data')
        cwl_file.flush()

        input_json.write('Some data')
        input_json.flush()

    resp_status = workflow_executor.execute.run(state=state, cwl_document=cwl_file, job_input_json=input_json,
                                                volume_name_prefix=volume_name_prefix, mount_folder=mount_folder,
                                                namespace=namespace, workflow_name=workflow_name)
    return resp_status



# def main(host, port, root_path, dataset):
#    uvicorn.run(app, host=host, port=port, root_path=root_path)

def main():
    uvicorn.run(app)


if __name__ == "__main__":
    main()
