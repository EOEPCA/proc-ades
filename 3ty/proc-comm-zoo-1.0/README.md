
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
![Build][build-shield]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/EOEPCA/template-svce">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">proc-comm-zoo</h3>

  <p align="center">
    Dockerize <a href="http://zoo-project.org/">zoo-project.org</a>
    <br />
    <a href="#table-of-contents"><strong>Explore the docs »</strong></a>
    <br />
    <a href="http://zoo-project.org/examples/otb-example.html">View zoo-project Demo</a>
    ·
    <a href="https://github.com/EOEPCA/proc-comm-zoo/issues">Report Bug</a>
    ·
    <a href="https://github.com/EOEPCA/proc-comm-zoo/issues">Request Feature</a>
  </p>
</p>

<!-- TABLE OF CONTENTS -->

## Table of Contents

- [About the Project](#about-the-project)
  - [Built With](#built-with)
  - [Resources](#resources)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Testing](#testing)
  - [eoepca distribution](#eoepca-distribution)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)

<!-- ABOUT THE PROJECT -->

## About The Project

The scripts included with the application prepare and build a Docker image of [WPS zoo-project][zoo-project-link].

The zoo-project version used is [1.7.0](http://zoo-project.org/dl/zoo-project-1.7.0.zip)

The WPS server has been configured to use the c/c++ languages.


### Built With

- [Docker](https://www.docker.com/)
- [autoconf](https://www.gnu.org/software/autoconf/)
- [configure](https://en.wikipedia.org/wiki/Configure_script)
- [make](http://man7.org/linux/man-pages/man1/make.1.html)

### Resources

- assets/main.cfg: configuration file copied into the Docker image
- assets/zoo/http: httpd configuration files

<!-- GETTING STARTED -->

## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites

- [Internet access](https://en.wikipedia.org/wiki/Internet_access)
- [Docker](https://www.docker.com/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))
- [curl](https://en.wikipedia.org/wiki/CURL)

### Installation

1. Get into Linux terminal

2. Clone the repo

```sh
git clone https://github.com/EOEPCA/proc-comm-zoo.git
```

3. Change local directory

```sh
cd proc-comm-zoo
```

5. Build application

```sh
./scripts/build.sh
```

If you are performing a local build the script creates a Docker Image:

```text
proc-comm-zoo:1.0
```

### Testing

1. Run local image

```sh
docker run -d --rm --name zoo  -p 7777:80 proc-comm-zoo:1.0
```

2. Send a request

```ssh
curl -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
```

### eoepca distribution

EOEPCA provides a ready-made Docker Image in its [DockerHub][eoepca-zoo]

## Usage

Download and run the Docker Image

### Running the template service

1. Run local build or [eoepca distribution][eoepca-zoo]

Local.

```sh
docker run -d --rm --name zoo  -p 7777:80 proc-comm-zoo:1.0
```

eoepca.

```sh
docker run -d --rm --name zoo  -p 7777:80 eoepca/proc-comm-zoo:latest
```

The Docker container also supports for OGC API - Processes:

```sh
curl -s -L "http://localhost:7777/wps3/processes" -H "accept: application/json"
```

```json
{
  "processes": [
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
    }
  ]
}
```

```sh
curl -s -L "http://localhost:7777/wps3/processes/longProcess" -H "accept: application/json"
```

```json
{
  "process": {
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
        "title": "Execute End Point",
        "href": "/watchjob/processes/longProcess/jobs/"
      }
    ],
    "inputs": [
      {
        "id": "sid",
        "title": "Service ID",
        "abstract": "A ZOO Service ID (unused).",
        "minOccurs": "0",
        "maxOccurs": "1",
        "input": {
          "literalDataDomains": [
            {
              "dataType": {
                "name": "integer"
              },
              "valueDefinition": {
                "anyValue": true
              }
            }
          ]
        }
      }
    ],
    "outputs": [
      {
        "id": "Result",
        "title": "ExecuteResponse document",
        "abstract": "The resulting ExecuteResponse document.",
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
        },
        "metadata": [
          {
            "title": "Demo XSL use case"
          }
        ]
      }
    ]
  }
}
```

```sh
curl -v -L -X POST "http://localhost:7777/wps3/processes/longProcess/jobs" -H  "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d "{\"inputs\":[{\"id\":\"string\"}],\"outputs\":[{\"format\":{\"mimeType\":\"string\",\"schema\":\"string\",\"encoding\":\"string\"},\"id\":\"Result\",\"transmissionMode\":\"value\"}],\"mode\":\"sync\",\"response\":\"raw\",\"subscriber\":{\"successUri\":\"string\",\"inProgressUri\":\"string\",\"failedUri\":\"string\"}}"
```

result

```txt
* About to connect() to localhost port 7777 (#0)
*   Trying 127.0.0.1...
* Connected to localhost (127.0.0.1) port 7777 (#0)
> POST /wps3/processes/longProcess/jobs HTTP/1.1
> User-Agent: curl/7.29.0
> Host: localhost:7777
> accept: application/json
> Prefer: respond-async
> Content-Type: application/json
> Content-Length: 266
>
* upload completely sent off: 266 out of 266 bytes
< HTTP/1.1 201 Created
< Date: Tue, 05 May 2020 08:32:28 GMT
< Server: Apache/2.4.6 (CentOS)
< X-Powered-By: ZOO@ZOO-Project
< Location: /watchjob/processes/longProcess/jobs/f4574dc8-8eaa-11ea-9279-0242ac110002
< Transfer-Encoding: chunked
< Content-Type: application/json;charset=UTF-8
<
* Connection #0 to host localhost left intact
```

```sh
curl -s -L "http://localhost:7777/wps3/processes/longProcess/jobs" -H "accept: application/json"
```

```json
[
  {
    "id": "f4574dc8-8eaa-11ea-9279-0242ac110002",
    "infos": {
      "status": "successful",
      "message": "ZOO-Kernel successfully run your service!",
      "links": [
        {
          "Title": "Status location",
          "href": "/watchjob/processes/longProcess/jobs/f4574dc8-8eaa-11ea-9279-0242ac110002"
        },
        {
          "Title": "Result location",
          "href": "/watchjob/processes/longProcess/jobs/f4574dc8-8eaa-11ea-9279-0242ac110002/result"
        }
      ]
    }
  }
]
```

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


## Contact

<!-- ACKNOWLEDGEMENTS -->

## Acknowledgements


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/EOEPCA/template-svce.svg?style=flat-square
[contributors-url]: https://github.com/EOEPCA/template-svce/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/EOEPCA/template-svce.svg?style=flat-square
[forks-url]: https://github.com/EOEPCA/template-svce/network/members
[stars-shield]: https://img.shields.io/github/stars/EOEPCA/template-svce.svg?style=flat-square
[stars-url]: https://github.com/EOEPCA/template-svce/stargazers
[issues-shield]: https://img.shields.io/github/issues/EOEPCA/template-svce.svg?style=flat-square
[issues-url]: https://github.com/EOEPCA/template-svce/issues
[license-shield]: https://img.shields.io/github/license/EOEPCA/template-svce.svg?style=flat-square
[license-url]: https://github.com/EOEPCA/template-svce/blob/master/LICENSE
[build-shield]: https://www.travis-ci.com/EOEPCA/template-svce.svg?branch=master
[product-screenshot]: images/screenshot.png
[zoo-project-link]: http://www.zoo-project.org/
[eoepca-zoo]: https://hub.docker.com/r/eoepca/proc-comm-zoo/tags