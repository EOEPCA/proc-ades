#!/usr/bin/env bash

NAMESPACE='eoepca'
REPOSITORY="https://github.com/EOEPCA/proc-ades.git#feature/EOEPCA-145"
CONTEXT="Dockers/eopca-base/"
NAME="proc-comm-zoo"
DOCKERNAME="proc-comm-zoo:1.0"


oc --namespace=${NAMESPACE} new-build "${REPOSITORY}"  \
--strategy=docker \
--no-output=false  --to-docker=false --name="${NAME}" --output=name  --dry-run=false \
--to=openshift/${DOCKERNAME}
