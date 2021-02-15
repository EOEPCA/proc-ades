# WorkflowExecutor Test

The workflow executor test uses the following python libraries:
- [**FastApi TestClient**](https://fastapi.tiangolo.com/tutorial/testing/) a test client for FastApi 
- [**pytest-kind**](https://pypi.org/project/pytest-kind/)
 which is a python package of [KIND](https://kind.sigs.k8s.io/) a *tool for running local Kubernetes clusters using Docker container “nodes”*.

The test consists on creating a lightweight kubernetes cluster using KIND and interacting with the Workflow Executor API using the FastApi TestClient.

# Installation and usage 

  - Create a conda environment which will contain all the dependencies needed for the test
  ```shell script
conda create -f environment.yml
  ```

  - Activate the conda environment
  ```shell script
conda activate workflow_executor_env
  ```

  - Run the test using the following command:
 ```shell script
python -m workflow_executor.test.fastapiserver_test
```