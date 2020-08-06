#!/usr/bin/env bash

NAMESPACE='eoepca'
REPOSITORY="https://github.com/EOEPCA/proc-ades#feature/EOEPCA-145"
CONTEXT="3ty/proc-comm-zoo-1.2-alpha"
NAME="proc-comm-zoo"
DOCKERNAME="proc-comm-zoo:1.0"

#oc --namespace=${NAMESPACE} new-build "${REPOSITORY}"  \
#--strategy=docker --context-dir=${CONTEXT} \
#--no-output=false  --to-docker=false --name="${NAME}" --output=name  --dry-run=false \
#--to=openshift/${DOCKERNAME}
#


oc  --namespace=${NAMESPACE}  new-app httpd~https://github.com/EOEPCA/proc-ades#feature/EOEPCA-145

#oc --namespace=${NAMESPACE}   new-app "https://github.com/EOEPCA/proc-ades#feature/EOEPCA-145"