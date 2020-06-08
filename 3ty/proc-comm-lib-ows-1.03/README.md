


[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/EOEPCA/proc-comm-lib-ows">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">proc-comm-lib-ows</h3>

  <p align="center">
    OWS base parser
    <br />
    <a href="https://github.com/EOEPCA/proc-comm-lib-ows"><strong>Explore the docs »</strong></a>
    <br />
    <a href="https://github.com/EOEPCA/proc-comm-lib-ows">View Demo</a>
    ·
    <a href="https://github.com/EOEPCA/proc-comm-lib-ows/issues">Report Bug</a>
    ·
    <a href="https://github.com/EOEPCA/proc-comm-lib-ows/issues">Request Feature</a>
    
  </p>
</p>

<!-- TABLE OF CONTENTS -->

## Table of Contents

- [About the Project](#about-the-project)
  - [Built With](#built-with)
  - [Travis Build](#travis-build)
  - [Manual Build](#manual-build)
    - [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Testing](#testing)
- [Documentation](#documentation)
- [Usage](#usage)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)

<!-- ABOUT THE PROJECT -->

## About The Project

The aim of the project is to provide a library for the parser of an OWS document. The [file describes](tests/application/assetes/ows1.xml) the elements necessary for the publication of an EOEPCA/ADES service.

The `feed` element defines the root of the search

```xml
<?xml version="1.0" encoding="UTF-8"?>
<feed xmlns:owc="http://www.opengis.net/owc/1.0" xmlns="http://www.w3.org/2005/Atom"
      xmlns:example="http://www.example.com">
    <entry>
```

The `Identifier` tag uniquely defines the element package ID
```xml
<feed xmlns:owc="http://www.opengis.net/owc/1.0" xmlns="http://www.w3.org/2005/Atom"
      xmlns:example="http://www.example.com">
    <entry>
        <identifier xmlns="http://purl.org/dc/elements/1.1/">application_package_opt_ar2_preproc</identifier>
        <title type="text">Application package for OPT MACHINE-2 pre-p
```

The content tag, the child of offering, defines

1. The CWL descriptor that represents the application(application/cwl)
1. The Docker Image that contains the application(application/vnd.docker.distribution.manifest.v1+json)

```xml
<owc:offering xmlns:owc="http://www.opengis.net/owc/1.0"
              code="http://www.opengis.net/spec/owc-atom/1.0/req/wps">
    <owc:content type="application/vnd.docker.distribution.manifest.v1+json"
                 href="docker-co.example.com/the-operation:0.0.1.2"/>
    <owc:content type="application/cwl"
                 href="https://catalog.example.com/api/app-test/_applications/app-test/the-operation/0.0.1.2/the-operation.0.0.1.2-application.cwl"/>

```

The parser examines the DescribeProcess operation

```xml
          href="https://catalog.example.com/api/app-test/_applications/app-test/the-operation/0.0.1.2/the-operation.0.0.1.2-application.cwl"/>
  <owc:operation code="DescribeProcess">
      <owc:result type="text/xml">
          <ProcessDescription xml
```

The document defines inputs `DataInputs` and outputs `ProcessOutputs`

```xml
<ows:Title>DLR Sentinel-2 pre-processing</ows:Title>
                    <ows:Abstract>ABSTRACT OPT OPERATION-2 pre-processing</ows:Abstract>
                    <DataInputs>

                        <wps:Input minOccurs="1">
                            <ows:Identifier>input_re
```

```xml
 <ProcessOutputs>

      <Output>
          <ows:Identifier>result_osd</ows:Identifier>
          <ows:Title>OpenSearch Description to the Results</ows:Title>
```

From the version v1.2 has been added the possibility to create the EOEPCA/ADES form a CWL file even defined in a OWS file descriptor

```xml
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
    <title type="text">Result for OpenSearch query over type * in index eoepca-apps</title>
    <id>https://catalog.terradue.com:443/eoepca-apps/search?uid=application_package_sample_app&amp;format=atom</id>
    <updated>2020-04-21T15:21:05.912964Z</updated>
    <link rel="self" type="application/atom+xml" title="Reference link" href="https://catalog.terradue.com/eoepca-apps/search?uid=application_package_sample_app&amp;format=atom"/>
    <link rel="search" type="application/opensearchdescription+xml" title="OpenSearch Description link" href="https://catalog.terradue.com/eoepca-apps/description"/>
    <entry>
        <id>https://catalog.terradue.com:443/eoepca-apps/search?format=atom&amp;uid=application_package_sample_app_cwl_only</id>
        <title type="text">Application package for a sample App - CWL only</title>
        <summary type="html">Application package a sample App - CWL only</summary>
        <published>2020-04-21T15:21:05.912583Z</published>
        <updated>2020-04-09T07:38:13.493912Z</updated>
        <author>
            <name>Terradue</name>
            <uri>https://www.terradue.com</uri>
            <email>support@terradue.com</email>
        </author>
        <link rel="self" type="application/atom+xml" title="Reference link" href="https://catalog.terradue.com/eoepca-apps/search?format=atom&amp;uid=application_package_sample_app"/>
        <content type="html">Application package a sample App - CWL only</content>
        <owc:offering xmlns:owc="http://www.opengis.net/owc/1.0" code="http://www.opengis.net/eoc/applicationContext/cwl">
            <owc:content type="application/cwl" href="https://raw.githubusercontent.com/EOEPCA/proc-comm-lib-ows/develop/tests/application/assets/cwl.yaml" />
        </owc:offering>

        <identifier xmlns="http://purl.org/dc/elements/1.1/">application_package_sample_app</identifier>
    </entry>
</feed>
```

```yaml
...
- class: Workflow
    id: test-WF-area # service id [WPS] map to wps:Input/ows:identifier
    label: test-WF area # title [WPS] map to wps:Input/ows:title
    doc: test-WF burned area with NDVI and NDWI # description [WPS] map to wps:Input/ows:abstract
    inputs:
      base_dir: Directory
      test_threshold: # parameter id [WPS] map to wps:Input/ows:identifier
        doc: TEST difference threshold # [WPS] maps to wps:Input/ows:abstract
        label: TEST difference threshold # [WPS] maps to wps:Input/ows:title
        type: string
      test2_threshold: # parameter id [WPS] map to wps:Input/ows:identifier
        doc: TEST2 difference threshold # [WPS] maps to wps:Input/ows:abstract
        label: TEST2 difference threshold # [WPS] maps to wps:Input/ows:title
        type: string
      post_event:
        type: File
        doc: Sentinel-bb Level-TEST post-event description # [WPS] maps to wps:Input/ows:abstract
        label: Sentinel-bb Level-TEST post-event title
        stac:catalog:
          stac:href: catalog.json # optional. Default to 'catalog.json'.
          stac:collection_id: post_event
      pre_event:
        type: File[]
        doc: Sentinel-bb Level-TEST pre-event description # [WPS] maps to wps:Input/ows:abstract
        label: Sentinel-bb Level-TEST pre-event title
        stac:catalog:
          stac:href: catalog.json # optional. Default to 'catalog.json'.
          stac:collection_id: pre_event
    outputs:
      results: # parameter id [WPS] map to wps:Output/ows:identifier
        label: Sentinel-bb Level-TEST post-event title
        outputSource:
          - step1/results
        type: File
        stac:catalog:
          stac:href: catalog.json # optional. Default to 'catalog.json'.
          stac:collection_id: pre_event
          stac:format: application/geo+json
    steps:

...
```

### Built With

- [CMAKE](https://cmake.org/)
- [googletest](https://github.com/google/googletest)

<!-- GETTING STARTED -->


### Travis Build

The library can be compiled using Travis. All necessary scripts are provided in the `travis` folder

To speed up the build phase of Travis we can use a compiler prepared by the EOEPCA team

You need to add an environment variable with reference to the docker image

```ini
BUILDER_ENV_IMAGE=eoepca/eoepca-build-cpp:1.0
```

### Manual Build

#### Prerequisites

- [Docker](https://www.docker.com/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))

To compile the library, a compiler that supports C++[14](https://en.cppreference.com/w/cpp/14)/[17](https://en.cppreference.com/w/cpp/17) is required.

EOEPCA team provides a ready-made Docker Image

```shel
docker pull eoepca/eoepca-build-cpp:1.0
```

If you want to create the Docker image yourself, the repository provides a Docker file to satisfy all requests (Dockerfile_builder)

```bash
docker build -t eoepca-build-cpp:1.0 -f Dockerfile_builder
```

## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites

- [Docker](https://www.docker.com/)
- [Linux bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell))

### Installation

1. Clone the repo

```sh
git clone https://github.com/EOEPCA/proc-comm-lib-ows.git
```

2. Change local directory

```sh
cd cd proc-comm-lib-ows
```

For this example we'll use the ready-made Docker Image `eoepca/eoepca-build-cpp:1.0`

3. Build application

```shell script
./scripts/build.sh
```

The library has been created `libeoepcaows.so`

### Testing

#### Prerequisites

- [LibXml2](http://xmlsoft.org/)
- [LibXslt](http://xmlsoft.org/)
- [libcurl](https://curl.haxx.se/libcurl/)

from the root of the repository

```shell
cd demo/
./libtest-test --gtest_break_on_failure
```

runs only the unit tests

## Documentation

The component documentation can be found at https://github.com/EOEPCA/proc-comm-lib-ows/

<!-- USAGE EXAMPLES -->

## Usage

The library was designed to be loaded into RUNTIME using the \*nix API [dlfcn.h](https://pubs.opengroup.org/onlinepubs/7908799/xsh/dlfcn.h.html)

`proc-comm-lib-ows/includes/eoepca/IMod.hpp` defines the interface with the operating system.

```c++

...

  explicit IModInterface(std::string path) : path_(std::move(path)) {
    handle = dlopen(path_.data(), RTLD_LAZY);
    if (!handle) {
      handle = nullptr;
      setValid(false);
      setLastError(std::string(dlerror()));
    }
    setValid(true);
    resetError();
  }

...

```

`includes/eoepca/owl/eoepcaows.hpp` implements IMod by adding connection methods

```c++
 public:
  long (*version)(void);
  void (*getParserName)(char*, int);
  EOEPCA::OWS::OWSContext* (*parseFromFile)(const char*);
  EOEPCA::OWS::OWSContext* (*parseFromMemory)(const char*, int);
  void (*releaseParameter)(EOEPCA::OWS::OWSContext*);
```

The library is imported using its path

```c++

  //*nux user.
  auto lib = std::make_unique<EOEPCA::EOEPCAows>("./libeoepcaows.so");
  if (!lib->IsValid()) {
    //Mac user
    lib = std::make_unique<EOEPCA::EOEPCAows>("./libeoepcaows.dylib");
  }

  std::cout << "LIB version: " << lib->version() << "\n";

```

Parse:

The entities used after the parser are described in `proc-comm-lib-ows/includes/eoepca/owl/owsparameter.hpp`. the main object is OWSContext

```c++
class OWSContext {
  std::list<std::unique_ptr<OWSEntry>> entries;

 public:

 ...
 ...
};

```

As described from [c++11 standard](https://en.cppreference.com/w/cpp/11), the main object was incapsuletend in a [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr) smart pointer. 

The library also provides the destructor for releasing the resources;

```c++
std::unique_ptr<EOEPCA::OWS::OWSContext,
                std::function<void(EOEPCA::OWS::OWSContext*)>>
    ptrContext(lib->parseFromFile(argv[1]), lib->releaseParameter);
```

### Running

### Prerequisites

- [LibXml2](http://xmlsoft.org/)
- [LibXslt](http://xmlsoft.org/)

To run the application has built a runner:

`proc-comm-lib-ows/src/run.cpp`

run:

```sh

mkdir run
cp build/runner build/libeoepcaows.so run/
cp tests/application/assets/ows1.xml  run/

cd run/

./runner ows1.xml
```

expected result:

```text
[qbert@mycase run]$ ./runner ows1.xml
LIB version: 1
LIB name: EOEPCA OWS Parser
Run:
********************************
application_package_dlr_s2_preproc
	application/vnd.docker.distribution.manifest.v1+json docker-co.example.com/the-operation:0.0.1.2
	application/cwl https://catalog.example.com/api/app-test/_applications/app-test/the-operation/0.0.1.2/the-operation.0.0.1.2-application.cwl
the-operation
APC APC-2 pre-processing
ABSTRACT APC APC-2 pre-processing
theParams SIZE INPUT: 4
theParams SIZE OUTPUT: 1
```

## Roadmap

See the [open issues](https://github.com/EOEPCA/proc-comm-lib-ows/issues) for a list of proposed features (and known issues).

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

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email

Project Link: [https://github.com/EOEPCA/proc-comm-lib-ows](https://github.com/EOEPCA/template-svce)

<!-- ACKNOWLEDGEMENTS -->

## Acknowledgements

- []()
- []()
- README.md is based on [this template](https://github.com/othneildrew/Best-README-Template) by [Othneil Drew](https://github.com/othneildrew).

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
[product-screenshot]: images/screenshot.png
