##
#
# setup:
#   - generate an ssh key to be used as an authentication key with `ssh-keygen` and call it `docker_ssh_rsa`
#   - make sure your current ssh agent has the identity file added
#     - $ eval `ssh-agent`
#     - $ ssh-add docker_ssh_rsa
#
# build: $ sudo docker build -t <you>/metadata_extractor .
#
# run: $ sudo docker run -it <you>/metadata_extractor  /bin/bash
#
#
##

FROM docker.terradue.com/c6-base-yarn-container:latest
MAINTAINER "Emmanuel Mathot" <emmanuel.mathot@terradue.com>

# install opensearch-client
RUN yum install -y mono opensearch-client MetadataExtractorClient
