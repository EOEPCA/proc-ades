<!--
*** 
*** To avoid retyping too much info. Do a search and replace for the following:
*** proc-ades, twitter_handle, email
-->

<!-- PROJECT SHIELDS -->
<!--
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
![Build][build-shield]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/EOEPCA/proc-ades">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">ADES building block</h3>

  <p align="center">
    Application Deployment and Execution Service (ADES) building block
    <br />
    <a href="https://github.com/EOEPCA/proc-ades"><strong>Explore the docs »</strong></a>
    <br />
    <a href="https://github.com/EOEPCA/proc-ades">View Demo</a>
    ·
    <a href="https://github.com/EOEPCA/proc-ades/issues">Report Bug</a>
    ·
    <a href="https://github.com/EOEPCA/proc-ades/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
## Table of Contents

- [Table of Contents](#table-of-contents)
- [About The Project](#about-the-project)
  - [Built With](#built-with)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Prerequisites Installation and Configuration](#prerequisites-installation-and-configuration)
  - [Build](#build)
  - [Run local image](#run-local-image)
  - [Eoepca Ades Deploy process](#eoepca-ades-deploy-process)
  - [Configure](#configure)
    - [argo.json](#argojson)
    - [Kubernetes Persistent Volume](#kubernetes-persistent-volume)
    - [Kubernetes Secret Environmental Variables](#kubernetes-secret-environmental-variables)
    - [The application](#the-application)
  - [Usage](#usage)
  - [Persistence](#persistence)
  - [Installation](#installation)
  - [Testing](#testing)
- [Usage](#usage-1)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://github.com/EOEPCA/)

The Processing & Chaining domain area provides an extensible repository of processing functions, tools and applications that can be discovered by search query, invoked individually, and utilised in workflows. ADES is responsible for the execution of the processing service as a OGC WPS 2.0 request within the target Exploitation Platform (i.e., the one that is close to the data).

### Built With

* [Terraform](https://terraform.io/)
* [Ansible](https://ansible.com)
* [Kubernetes](https://kubernetes.io)
* [Minikube](https://github.com/kubernetes/minikube)
* [Docker](https://docker.com)

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites

Things you need to use the software and how to install them.

- [Internet access](https://en.wikipedia.org/wiki/Internet_access)
- [Docker](https://docs.docker.com/engine/install/centos/)
- [Minikube](https://kubernetes.io/docs/tasks/tools/install-minikube/)
- [Kubectl](https://kubernetes.io/docs/tasks/tools/install-kubectl/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))
- [curl](https://en.wikipedia.org/wiki/CURL)
- [Terraform](https://terraform.io/) 
- [Ansible](https://docs.ansible.com/ansible/latest/installation_guide/intro_installation.html)

### Prerequisites Installation and Configuration

- [Docker](https://docs.docker.com/engine/install/centos/)
  
To install and start Docker Engine, execute the following commands

```sh
sudo yum install -y docker-ce docker-ce-cli containerd.io
sudo systemctl start docker
```

- [Minikube]( https://kubernetes.io/docs/tasks/tools/install-minikube/
)

Execute the following command to install Minikube
```sh
curl -Lo minikube https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64 && chmod +x minikube
sudo mkdir -p /usr/local/bin/
sudo install minikube /usr/local/bin/
```
For further information please refer to https://kubernetes.io/docs/tasks/tools/install-minikube/

Run the following command to start up a local Kubernetes cluster
```sh
minikube start --vm-driver=none
```

Once minikube start finishes, run the command below to check the status of the cluster:
```sh
minikube status
```
If your cluster is running, the output from minikube status should be similar to:
```sh
host: Running
kubelet: Running
apiserver: Running
kubeconfig: Configured
```

- Kubectl
  
To install Kubectl execute the following commands:
```sh
cat <<EOF > /etc/yum.repos.d/kubernetes.repo
[kubernetes]
name=Kubernetes
baseurl=https://packages.cloud.google.com/yum/repos/kubernetes-el7-x86_64
enabled=1
gpgcheck=1
repo_gpgcheck=1
gpgkey=https://packages.cloud.google.com/yum/doc/yum-key.gpg https://packages.cloud.google.com/yum/doc/rpm-package-key.gpg
EOF
yum install kubectl-1.17.0
```
Start the proxy to the Kubernetes API server:
```sh
kubectl proxy --port=8080 &

```
For further information please refer to https://kubernetes.io/docs/tasks/access-kubernetes-api/http-proxy-access-api/

- Argo Workflow

To install Argo Workflows execute the following commands:
```sh
kubectl create namespace argo
kubectl apply -n argo -f https://raw.githubusercontent.com/argoproj/argo/stable/manifests/install.yaml
```

Grant Admin priviledges
Grant the default ServiceAccount admin privileges (i.e., we will bind the admin Role to the default ServiceAccount of the current namespace):
```sh
kubectl create rolebinding default-admin --clusterrole=admin --serviceaccount=default:default
```
For further information refer to https://argoproj.github.io/docs/argo/getting-started.html

Start argo server port forwarding
```sh
kubectl -n argo port-forward deployment/argo-server 2746:2746 & 
```

### Build

1. Get into Linux terminal

2. Clone the repository

```sh
git clone https://github.com/EOEPCA/proc-ades.git
```

3. Change local directory

```sh
cd proc-ades
```

4. Build application

```sh
./scripts/build.sh
```

If you are performing a local build the script creates two Docker Images:

```text
eoepca-ades-core:1.0
proc-comm-zoo:1.0
```

### Run local image

Run:

```sh
docker run --rm  -d --name zoo -p 7777:80   eoepca-ades-core:1.0
```

2. Send a GetCapabilities request

```ssh
curl -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
```

```xml
<?xml version="1.0" encoding="UTF-8"?>
<wps:Capabilities xmlns:ows="http://www.opengis.net/ows/1.1" xmlns:wps="http://www.opengis.net/wps/1.0.0" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsGetCapabilities_response.xsd" service="WPS" version="1.0.0" xml:lang="en-US">
  <ows:ServiceIdentification>
    <ows:Title>Ellip-WPS</ows:Title>
    <ows:Fees>None</ows:Fees>
    <ows:AccessConstraints>none</ows:AccessConstraints>
  </ows:ServiceIdentification>
  <ows:ServiceProvider>
    <ows:ProviderName>xxxx</ows:ProviderName>
    <ows:ProviderSite xlink:href="https://www.xxxxx.com"/>
    <ows:ServiceContact>
      <ows:IndividualName>Operations Support team</ows:IndividualName>
      <ows:PositionName>xxxx</ows:PositionName>
      <ows:ContactInfo>
        <ows:Phone>
          <ows:Voice>+xxxxx</ows:Voice>
          <ows:Facsimile>False</ows:Facsimile>
        </ows:Phone>
        <ows:Address>
          <ows:DeliveryPoint>xxxxx</ows:DeliveryPoint>
          <ows:City>xx</ows:City>
          <ows:AdministrativeArea>False</ows:AdministrativeArea>
          <ows:PostalCode>xxx</ows:PostalCode>
          <ows:Country>IT</ows:Country>
          <ows:ElectronicMailAddress>support@xxxx.com</ows:ElectronicMailAddress>
        </ows:Address>
      </ows:ContactInfo>
    </ows:ServiceContact>
  </ows:ServiceProvider>
  <ows:OperationsMetadata>
    <ows:Operation name="GetCapabilities">
      <ows:DCP>
        <ows:HTTP>
          <ows:Get xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
          <ows:Post xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
        </ows:HTTP>
      </ows:DCP>
    </ows:Operation>
    <ows:Operation name="DescribeProcess">
      <ows:DCP>
        <ows:HTTP>
          <ows:Get xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
          <ows:Post xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
        </ows:HTTP>
      </ows:DCP>
    </ows:Operation>
    <ows:Operation name="Execute">
      <ows:DCP>
        <ows:HTTP>
          <ows:Get xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
          <ows:Post xlink:href="http://localhost:7777/zoo-bin/zoo_loader.cgi"/>
        </ows:HTTP>
      </ows:DCP>
    </ows:Operation>
  </ows:OperationsMetadata>
  <wps:ProcessOfferings>
    <wps:Process wps:processVersion="1.0.0">
      <ows:Identifier>eoepcaadesundeployprocess</ows:Identifier>
      <ows:Title>Eoepca Deploy Process</ows:Title>
      <ows:Abstract>This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    </wps:Process>
    <wps:Process wps:processVersion="1">
      <ows:Identifier>longProcess</ows:Identifier>
      <ows:Title>Demo long process. </ows:Title>
      <ows:Abstract>This service doesn't do anything except taking its time, it demonstrates how to use the updateStatus function from your ZOO Service. </ows:Abstract>
      <ows:Metadata xlink:title="Demo GetStatus request"/>
    </wps:Process>
    <wps:Process wps:processVersion="1">
      <ows:Identifier>GetStatus</ows:Identifier>
      <ows:Title>Produce an updated ExecuteResponse document. </ows:Title>
      <ows:Abstract>Create an ExecuteResponse document from a sid (Service ID), it will use the niternal ZOO Kernel mechanisms to access the current status from a running Service and update the percentCompleted from the original backup file used by the ZOO Kernel when running a Service in background. </ows:Abstract>
      <ows:Metadata xlink:title="Demo GetStatus request"/>
    </wps:Process>
    <wps:Process wps:processVersion="1.0.0">
      <ows:Identifier>eoepcaadesdeployprocess</ows:Identifier>
      <ows:Title>Eoepca Deploy Process</ows:Title>
      <ows:Abstract>This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.</ows:Abstract>
    </wps:Process>
  </wps:ProcessOfferings>
  <wps:Languages>
    <wps:Default>
      <ows:Language>en-US</ows:Language>
    </wps:Default>
    <wps:Supported>
      <ows:Language>en-US</ows:Language>
      <ows:Language>en-GB</ows:Language>
    </wps:Supported>
  </wps:Languages>
</wps:Capabilities>
```

3. Send a OGC API - Processes request 

```ssh
curl -s -L "http://localhost:7777/wps3/processes" -H "accept: application/json"
```

```json
{
  "processes": [
    {
      "id": "eoepcaadesundeployprocess",
      "title": "Eoepca Deploy Process",
      "abstract": "This method will undeploy an application encapsulated within a Docker container as a process accessible through the WPS interface.",
      "version": "1.0.0",
      "jobControlOptions": [
        "sync-execute",
        "async-execute",
        "dismiss"
      ],
      "outputTransmission": [
        "value",
        "reference"
      ],
      "links": [
        {
          "rel": "canonical",
          "type": "application/json",
          "title": "Process Description",
          "href": "/watchjob/processes/eoepcaadesundeployprocess/"
        }
      ]
    },
    {
      "id": "longProcess",
      "title": "Demo long process. ",
      "abstract": "This service doesn't do anything except taking its time, it demonstrates how to use the updateStatus function from your ZOO Service. ",
      "version": "1.0.0",
      "jobControlOptions": [
        "sync-execute",
        "async-execute",
        "dismiss"
      ],
      "outputTransmission": [
        "value",
        "reference"
      ],
      "links": [
        {
          "rel": "canonical",
          "type": "application/json",
          "title": "Process Description",
          "href": "/watchjob/processes/longProcess/"
        }
      ]
    },
    {
      "id": "GetStatus",
      "title": "Produce an updated ExecuteResponse document. ",
      "abstract": "Create an ExecuteResponse document from a sid (Service ID), it will use the niternal ZOO Kernel mechanisms to access the current status from a running Service and update the percentCompleted from the original backup file used by the ZOO Kernel when running a Service in background. ",
      "version": "1.0.0",
      "jobControlOptions": [
        "sync-execute",
        "async-execute",
        "dismiss"
      ],
      "outputTransmission": [
        "value",
        "reference"
      ],
      "links": [
        {
          "rel": "canonical",
          "type": "application/json",
          "title": "Process Description",
          "href": "/watchjob/processes/GetStatus/"
        }
      ]
    },
    {
      "id": "eoepcaadesdeployprocess",
      "title": "Eoepca Deploy Process",
      "abstract": "This method will deploy an application encapsulated within a Docker container as a process accessible through the WPS interface.",
      "version": "1.0.0",
      "jobControlOptions": [
        "sync-execute",
        "async-execute",
        "dismiss"
      ],
      "outputTransmission": [
        "value",
        "reference"
      ],
      "links": [
        {
          "rel": "canonical",
          "type": "application/json",
          "title": "Process Description",
          "href": "/watchjob/processes/eoepcaadesdeployprocess/"
        }
      ]
    }
  ]
}
```

### Eoepca Ades Deploy process

Our tutorial will start in the dev-env-argo environment

1) Start "eoepca-ades-core"

```shell script
docker run --rm  -d --name zoo -p 7777:80 --network host  eoepca-ades-core:1.0
```

with the option `--network host` the port 7777 will be ignored

2) run a getProcess to test the installation:

```shell script
curl -s -L "http://localhost/wps3/processes" -H "accept: application/json"
```


3) install the new service

create json file parameter `deploy.json`

```json
{
  "inputs": [
    {
      "id": "applicationPackage",
      "input": {
        "format": {
          "mimeType": "application/xml"
        },
        "value": {
          "inlineValue": "https://raw.githubusercontent.com/EOEPCA/proc-ades/develop/test/sample_apps/metadata_extractor/ows.xml"
        }
      }
    }
  ],
  "outputs": [
    {
      "format": {
        "mimeType": "string",
        "schema": "string",
        "encoding": "string"
      },
      "id": "deployResult",
      "transmissionMode": "value"
    }
  ],
  "mode": "async",
  "response": "raw"
}
```

run:

```shell script
curl -v -L -X POST "http://localhost/wps3/processes/eoepcaadesdeployprocess/jobs" -H  \
  "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d@deploy.json
```

```text
>   "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d@deploy.json
* About to connect() to localhost port 7777 (#0)
*   Trying 127.0.0.1...
* Connected to localhost (127.0.0.1) port 7777 (#0)
> POST /wps3/processes/eoepcaadesdeployprocess/jobs HTTP/1.1
> User-Agent: curl/7.29.0
> Host: localhost:7777
> accept: application/json
> Prefer: respond-async
> Content-Type: application/json
> Content-Length: 543
>
* upload completely sent off: 543 out of 543 bytes
< HTTP/1.1 201 Created
< Date: Mon, 25 May 2020 14:11:43 GMT
< Server: Apache/2.4.6 (CentOS)
< X-Powered-By: ZOO@ZOO-Project
< Location: /watchjob/processes/eoepcaadesdeployprocess/jobs/a93616e8-9e91-11ea-a71d-0242ac110002
< Transfer-Encoding: chunked
< Content-Type: application/json;charset=UTF-8
<
* Connection #0 to host localhost left intact
```

running again the getProcess we can read a new service:

```json
{
      "id": "eo_metadata_generation_1_0",
      "title": "Earth Observation Metadata Generation",
      "abstract": "Earth Observation Metadata Generation",
      "version": "1.0.0.0",
      "jobControlOptions": [
        "sync-execute",
        "async-execute",
        "dismiss"
      ],
      "outputTransmission": [
        "value",
        "reference"
      ],
      "links": [
        {
          "rel": "canonical",
          "type": "application/json",
          "title": "Process Description",
          "href": "/watchjob/processes/eo_metadata_generation_1_0/"
        }
      ]
    }
```



4) Looking for the our service argo:
 
```shell script
curl -s -L "http://localhost/wps3/processes/eo_metadata_generation_1_0" -H "accept: application/json"
```

```json
{
  "process": {
    "id": "eo_metadata_generation_1_0",
    "title": "Earth Observation Metadata Generation",
    "abstract": "Earth Observation Metadata Generation",
    "version": "1.0.0.0",
    "jobControlOptions": [
      "sync-execute",
      "async-execute",
      "dismiss"
    ],
    "outputTransmission": [
      "value",
      "reference"
    ],
    "links": [
      {
        "rel": "canonical",
        "type": "application/json",
        "title": "Execute End Point",
        "href": "/watchjob/processes/eo_metadata_generation_1_0/jobs/"
      }
    ],
    "inputs": [
      {
        "id": "input_file",
        "title": "EO input file",
        "abstract": "Mandatory input file to generate metadata for",
        "minOccurs": "1",
        "maxOccurs": "0",
        "input": {
          "formats": [
            {
              "default": true,
              "mimeType": "application/json"
            },
            {
              "default": false,
              "mimeType": "application/json"
            },
            {
              "default": false,
              "mimeType": "application/yaml"
            },
            {
              "default": false,
              "mimeType": "application/atom+xml"
            },
            {
              "default": false,
              "mimeType": "application/opensearchdescription+xml"
            }
          ]
        }
      }
    ],
    "outputs": [
      {
        "id": "results",
        "title": "Outputs blah blah",
        "abstract": "results",
        "output": {
          "literalDataDomains": [
            {
              "dataType": {
                "name": "string"
              },
              "valueDefinition": {
                "anyValue": true
              }
            }
          ]
        }
      }
    ]
  }
}

```

### Configure

#### argo.json

The 'assets/argo.json' file is used to configure the connection to Kubernetes and Argo

```json
{
	"argopath": "/apis/argoproj.io/v1alpha1",
	"k8Url": "http://localhost:2746",
	"argoUrl": "http://localhost:8080",
	"stage":{
		"in":{
			"docker":"docker/host.me",
		},
		"out":{
			"docker":"docker/host.me",
      "webdav_endpoint":"https://nx10438.your-storageshare.de/remote.php/dav/files/eoepca-demo-storage"
		}
	}
}
```

*argopath*: defines the url path to the argo workflow api

*k8Url*: defines the host of the kubernetes api

*argoUrl*: defines the host of the  argo workflow api


  
*stage* --> *in* --> *docker* : defines the docker image used for the stage in node

*stage* --> *out* -->  *docker*: defines the docker image used for the stage out node

*stage* --> *out* --> *webdav_endpoint* : defines the webdav external storage endpoint 

#### Kubernetes Persistent Volume
	
To share a workspace among the various nodes of the workflow, we need a common filesystem, for this application we will use [Kubernetes Persistent Volume and Persistent Volume Claim](https://kubernetes.io/docs/concepts/storage/persistent-volumes/). In order to create one, write the file **eoepca-pv-and-pvc.yaml** with the following content:

```
apiVersion: v1
kind: PersistentVolume
metadata:
  name: eoepca-pv
  labels:
    type: local
spec:
  storageClassName: manual
  capacity:
    storage: 10Gi
  accessModes:
    - ReadWriteOnce
  persistentVolumeReclaimPolicy: Retain
  storageClassName: standard
  hostPath:
    path: "/mnt/data"

---

apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: eoepca-pvc
spec:
  accessModes:
  - ReadWriteOnce
  storageClassName: standard
  resources:
    requests:
      storage: 3Gi
```
and run the following  command:
```
 kubectl create -f eoepca-pv-and-pvc.yaml
```

#### Kubernetes Secret Environmental Variables

The stage-out phase of the workflow consists in storing the results of the processing job in an external storage provided by Nextcloud (https://nextcloud.com/). This storage solution provides a Webdav interface which is an extension of the HTTP protocol that allows clients to perform authoring operations. 
In order to keep the credentials to the external storage hidden, we are using Kubernetes "Secret" environmental variables. For further infomation please refer to Kubernetes officiale guide (https://kubernetes.io/docs/tasks/inject-data-application/distribute-credentials-secure/).

To store your credentials into Kubernetes secret environmental variables please execute the following commands:

- Use a base64 encoding tool to convert your username and password to a base64 representation.
```sh
echo -n 'user123' | base64
echo -n '39528$vdg7Jb' | base64
```

The output shows that the base-64 representation of your username and password.


- Create Kubernetes secret configuration secret.yaml

```yaml
apiVersion: v1
kind: Secret
metadata:
  name: eoepcawebdavsecret
type: Opaque
data:
  username: ZW9lcGNhLWRlbW8tc3R
  password: VWZqaDEwe
```

- Create the Kubernetes secret using the following command:

```sh
kubectl create -f secret.yaml
```
#### The application
The docker images used for this sample application can be found at the following address:
- [https://hub.docker.com/r/blasco/eoepca-eo-tools](https://hub.docker.com/r/blasco/eoepca-eo-tools)
- [https://hub.docker.com/r/blasco/stagein](https://hub.docker.com/r/blasco/stagein)
- [https://hub.docker.com/r/blasco/stageout](https://hub.docker.com/r/blasco/stageout)
  
For further information, please find their dockerfiles under *assets/dockerfiles*.

### Usage
Once the deploy and configuration is complete, you can proceed running the application that the Ades service provides and that is defined in the following (cwl)(https://github.com/EOEPCA/proc-ades/blob/develop/test/sample_apps/metadata_extractor/workflow.cwl).


Create the job parameter file *app_params.json* as follows:
```json
{
  "inputs": [
    {
      "id": "input_file",
      "input": {
        "format": {
          "mimeType": "application/atom+xml"
        },
        "value": {
          "inlineValue": "https://catalog.terradue.com/sentinel3/search?uid=S3B_SL_1_RBT____20200520T050759_20200520T051059_20200520T060015_0179_039_105_0360_LN2_O_NR_004"
        }
      }
    }
  ],
  "outputs": [
    {
      "format": {
        "mimeType": "string",
        "schema": "string",
        "encoding": "string"
      },
      "id": "results",
      "transmissionMode": "value"
    }
  ],
  "mode": "async",
  "response": "raw"
} 
```

Run:
```sh
curl -v -L -X POST "http://localhost/wps3/processes/eo_metadata_generation_1_0/jobs" -H  \
  "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d@app_params.json
```
Get Argo Jobs:

```shell script
curl  -v  -s -L "http://localhost/wps3/processes/eo_metadata_generation_1_0/jobs" -H "accept: application/json" 
```

```text
*   Trying 127.0.0.1...
* TCP_NODELAY set
* Connected to localhost (127.0.0.1) port 80 (#0)
> GET /wps3/processes/eo_metadata_generation_1_0/jobs HTTP/1.1
> Host: localhost
> User-Agent: curl/7.58.0
> accept: application/json
> 
< HTTP/1.1 200 OK 
< Date: Mon, 25 May 2020 15:53:53 GMT
< Server: Apache/2.4.6 (CentOS)
< X-Powered-By: ZOO@ZOO-Project
< Transfer-Encoding: chunked
< Content-Type: application/json;charset=UTF-8
< 
{ [421 bytes data]
* Connection #0 to host localhost left intact
[
  {
    "id": "06223bbe-9e9e-11ea-8f64-a0c5899f98fe",
    "infos": {
      "status": "successful",
      "message": "ZOO-Kernel successfully run your service!",
      "links": [
        {
          "Title": "Status location",
          "href": "/watchjob/processes/eo_metadata_generation_1_0/jobs/06223bbe-9e9e-11ea-8f64-a0c5899f98fe"
        },
        {
          "Title": "Result location",
          "href": "/watchjob/processes/eo_metadata_generation_1_0/jobs/06223bbe-9e9e-11ea-8f64-a0c5899f98fe/result"
        }
      ]
    }
  }
]

```

and Get Result

```shell script
curl  -v  -s -L "http://localhost/watchjob/processes/eo_metadata_generation_1_0/jobs/06223bbe-9e9e-11ea-8f64-a0c5899f98fe/result" -H "accept: application/json"
```

```text
*   Trying 127.0.0.1...
* TCP_NODELAY set
* Connected to localhost (127.0.0.1) port 80 (#0)
> GET /watchjob/processes/eo_metadata_generation_1_0/jobs/06223bbe-9e9e-11ea-8f64-a0c5899f98fe/result HTTP/1.1
> Host: localhost
> User-Agent: curl/7.58.0
> accept: application/json
> 
< HTTP/1.1 200 OK
< Date: Mon, 25 May 2020 15:46:09 GMT
< Server: Apache/2.4.6 (CentOS)
< Transfer-Encoding: chunked
< Content-Type: application/json;charset=UTF-8
< 
{ [178 bytes data]
* Connection #0 to host localhost left intact
{
  "outputs": [
    {
      "id": "results",
      "value": {
        "results": "https://nx10438.your-storageshare.de/remote.php/dav/files/eoepca-demo-storage/fad0bd5c-a95c-11ea-9a94-08002798a283eo-metadata-generation-1-0/S3B_SL_1_RBT____20200520T050759_20200520T051059_20200520T060015_0179_039_105_0360_LN2_O_NR_004.atom"
      }
    }
  ]
}

```
To download the result file use the following command:

```
curl -u eoepca-demo-storage:somepassword123 https://nx10438.your-storageshare.de/remote.php/dav/files/eoepca-demo-storage/fad0bd5c-a95c-11ea-9a94-08002798a283eo-metadata-generation-1-0/S3B_SL_1_RBT____20200520T050759_20200520T051059_20200520T060015_0179_039_105_0360_LN2_O_NR_004.atom -o /tmp/metadata.atom
```

### Persistence

You can create a persistence of the installed services and the results of the processes.

The applications, inside the Docker container, are run by the user `apache` with
id `48`. This information defines the types of permissions that must be assigned to the shared filesystem.

I paths esterni da condividere sono:

1. /zooservices/
1. /var/www/html/res/

```sh
cd /tmp/

mkdir zooservices/
mkdir -p  res/statusInfos

sudo chown 48:48  zooservices
sudo chown 48:48  res
sudo chown 48:48  res/statusInfos
```
and run the Ades Image

sudo docker run --rm  -d --name zoo -p 7777:80  -v $PWD/zooservices:/zooservices/  -v  $PWD/res:/var/www/html/res   eoepca-ades-core:1.0

and now install a new service following the example in the section `Eoepca Ades Deploy process`.

The results and the new library can be shown into the folders `res` and `zooservices`

```txt
[folder@host tmp]$ ls res/
eoepcaadesdeployprocess_fea18f9c-a65f-11ea-b533-0242ac110002_error.log   fea18f9c-a65f-11ea-b533-0242ac110002_lenv.cfg
eoepcaadesdeployprocess_fea18f9c-a65f-11ea-b533-0242ac110002.json        fea18f9c-a65f-11ea-b533-0242ac110002.pid
eoepcaadesdeployprocess_fea18f9c-a65f-11ea-b533-0242ac110002.xml         fea18f9c-a65f-11ea-b533-0242ac110002.sid
eoepcaadesdeployprocess_final_fea18f9c-a65f-11ea-b533-0242ac110002.json  statusInfos


[folder@host tmp]$ ls zooservices/
eoepcaadesdeployprocess.zcfg    eo_metadata_generation_1_0.yaml  eo_metadata_generation_1_0.zo  libargo_interface.so  libepcatransactional.zo  main.cfg  wps_status.zo
eoepcaadesundeployprocess.zcfg  eo_metadata_generation_1_0.zcfg  GetStatus.zcfg                 libeoepcaargo.so      longProcess.zcfg         oas.cfg
```

### Installation

- Local environment - It creates a testing environment in a Minikube cluster deployed on the local machine  
1. `cd terraform/test`
2. `terraform init`
3. `terraform apply --auto-approve`

- Staging environment - It creates a testing environment on an Openstack environment provided by a Network-of-Resources provider (e.g. a DIAS platform)
1. [Register yourself into openstack provider and obtain cloud.key for access]
2. [Obtain detailed credential for accessing the environment and modify terraform/staging/variables.tf with this information]
3. `cd terraform/staging`
4. `terraform init`
5. `terraform apply --auto-approve`
6. [Output is master IP]
7. [Modify provisioning/hosts file to include the obtained master IP address]
8. `ansible-playbook provisioning/playbook.yml --inventory provisioning/hosts --limit masters --user eouser --private-key ~/.ssh/cloud.key`
9. [TODO: remote installation of kubernetes cluster]
10. [This needs ssh-ing into the master node, downloading the source code repository and terraforming it locally]
 
### Testing

Once installed, developers can deploy environments for these pipelines:
- local test: executing `sh travis/test_template-service.sh` will create the test environment, deploy a sample service and perform simple acceptance tests on that service.

- [TODO] remote integration: executing `sh travis/stage_template-service.sh` will create the staging environment, deploy a sample service and perform simple acceptance tests on that service. 

<!-- USAGE EXAMPLES -->
## Usage

Currently, only the design documentation is produced out of this repository.

<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/EOEPCA/proc-ades/issues) for a list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the Apache-2.0 License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

Terradue - [@terradue](https://twitter.com/terradue) - info@terradue.com

Project Link: [https://github.com/EOEPCA/proc-ades](https://github.com/EOEPCA/proc-ades)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* README.md is based on [this template](https://github.com/othneildrew/Best-README-Template) by [Othneil Drew](https://github.com/othneildrew).



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/EOEPCA/proc-ades.svg?style=flat-square
[contributors-url]: https://github.com/EOEPCA/proc-ades/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/EOEPCA/proc-ades.svg?style=flat-square
[forks-url]: https://github.com/EOEPCA/proc-ades/network/members
[stars-shield]: https://img.shields.io/github/stars/EOEPCA/proc-ades.svg?style=flat-square
[stars-url]: https://github.com/EOEPCA/proc-ades/stargazers
[issues-shield]: https://img.shields.io/github/issues/EOEPCA/proc-ades.svg?style=flat-square
[issues-url]: https://github.com/EOEPCA/proc-ades/issues
[license-shield]: https://img.shields.io/github/license/EOEPCA/proc-ades.svg?style=flat-square
[license-url]: https://github.com/EOEPCA/proc-ades/blob/master/LICENSE
[build-shield]: https://www.travis-ci.com/EOEPCA/proc-ades.svg?branch=master
[product-screenshot]: images/screenshot.png
