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
- [Getting Started & Usage](#getting-started--usage)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://github.com/EOEPCA/)

The Processing & Chaining domain area provides an extensible repository of processing functions, tools and applications that can be discovered by search query, invoked individually, and utilised in workflows. ADES is responsible for the execution of the processing service through both a [OGC WPS 1.0 & 2.0 OWS service](https://www.ogc.org/standards/wps) and an [OGC Processes REST API](https://github.com/opengeospatial/wps-rest-binding). The processing request are executed within the target Exploitation Platform (i.e., the one that is close to the data).

The ADES software uses [ZOO-Project](http://zoo-project.org/) as the main framework for exposing the OGC compliant web services. The [ZOO-kernel](http://zoo-project.org/docs/kernel/) powering the web services is included in the software package.

The ADES functions are designed to perform the processing and chaining function on a [Kubernetes](https://kubernetes.io) cluster using the [ARGO workflow](https://argoproj.github.io/) extension. Argo is a robust workflow engine for Kubernetes that enables the implementation of each step in a workflow as a container. It provides simple, flexible mechanisms for specifying constraints between the steps in a workflow and artifact management for linking the output of any step as an input to subsequent steps.



<!-- GETTING STARTED -->
## Getting Started & Usage

The full getting started guide start in the [Wiki home page](https://github.com/EOEPCA/proc-ades/wiki)

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
