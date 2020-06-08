FROM docker.terradue.com/c6-base-yarn-container:latest
MAINTAINER "Blasco Brauzzi" <blasco.brauzzi@terradue.com>

# install MetadataExtractorClient
RUN yum install -y MetadataExtractorClient tree ; yum clean all

COPY process_s3_metadata /usr/bin/eoepca-metadata-extractor
RUN chmod +x /usr/bin/eoepca-metadata-extractor
