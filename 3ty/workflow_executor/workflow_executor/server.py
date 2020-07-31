from types import SimpleNamespace

from flask import Flask
from flask import request

from workflow_executor import prepare

app = Flask(__name__)

@app.route('/')
def index():
    return "Hello, World bla!"


@app.route('/prepare/<namespace>/<int:volumeSize>/<volumeName>')
def preparePost(namespace,volumeSize,volumeName):
    # kubeconfig path
    kubeconfig = "/home/bla/.kube/config" #request.args.get('kubeconfig')

    print(namespace)
    print(volumeSize)
    print(volumeName)

    prepare_params={}
    prepare_params["namespace_name"]=namespace
    prepare_params["volumeSize"]=volumeSize
    prepare_params["volumeName"]=volumeName

    prepare_params["config"]=kubeconfig

    prepare.run(SimpleNamespace(**prepare_params))

    return "OK"








if __name__ == '__main__':
    app.run(debug=True)
