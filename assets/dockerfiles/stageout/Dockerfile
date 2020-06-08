FROM centos/python-36-centos7
MAINTAINER "Blasco Brauzzi" <blasco.brauzzi@terradue.com>
USER root

COPY eoepca_webdav_client /usr/bin/eoepca_webdav_client
RUN  chmod +x /usr/bin/eoepca_webdav_client
RUN pip install webdavclient3