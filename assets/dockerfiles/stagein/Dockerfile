FROM docker.terradue.com/c6-base-yarn-container:latest
MAINTAINER "Blasco Brauzzi" <blasco.brauzzi@terradue.com>

RUN yum install -y tree wget gcc ; yum clean all

COPY stagein /usr/bin/stagein
RUN chmod +x /usr/bin/stagein

