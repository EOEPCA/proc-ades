#!/usr/bin/env bash

# fail fast settings from https://dougrichardson.org/2018/08/03/fail-fast-bash-scripting.html
set -euov pipefail

source travis/variables.sh




echo "Run container "
docker run --rm  --name hw  ${EOEPCA_REPOSITORY}/${SERVICE_NAME}:$buildTag





