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
  - [Build](#build)
  - [Run local image](#run-local-image)
  - [Eoepca Ades Deploy Process](#eoepca-ades-deploy-process)
  - [Configure](#configure)
  - [Persistence](#persistence)
  - [Installation](#installation)
  - [Testing](#testing)
- [Usage](#usage)
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
- [Docker](https://www.docker.com/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))
- [curl](https://en.wikipedia.org/wiki/CURL)
- [Terraform](https://terraform.io/) 
- [Ansible](https://docs.ansible.com/ansible/latest/installation_guide/intro_installation.html)

### Build

1. Get into Linux terminal

2. Clone the repository

```sh
git clone https://github.com/EOEPCA/proc-ades.git
```

3. Change local directory

```sh
cd ades-core-engine-api-framework
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

The 'assets/argo.json' file is used to configure the connection to Kubernetes and Argo

```json
{
	"argopath": "/apis/argoproj.io/v1alpha1",
	"k8Url": "http://localhost:2746",
	"argoUrl": "http://localhost:8080",
	"stage":{
		"credential":{
			"user":"",
			"password":""
		},
		"in":{
			"docker":"docker/host.me",
			"credential":{
				"user":"",
				"password":""
			}
		},
		"out":{
			"docker":"docker/host.me",
			"credential":{
				"user":"",
				"password":""
			}
		}
	}
}
```

*argopath*: defines.....
*k8Url*: defines.....
*argoUrl*: defines.....
*stage*
  
*credential* --> *user*: defines ...
*credential* --> *password*: defines ...


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
