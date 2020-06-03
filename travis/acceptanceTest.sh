#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh


if [ "${TRAVIS}" == "true" ]; then
  docker pull "${EOEPCA_ADES_ZOO}"

  sudo docker run --rm  -d --name zoo -p 7777:80 ${EOEPCA_ADES_ZOO}

  sleep 15

  echo "run: bin/transational/01_getProcess.sh"
  ./bin/transational/01_getProcess.sh
  echo ""
  sleep 2

  echo "run: bin/transational/02_RunAsync.sh"
  ./bin/transational/02_RunAsync.sh bin/transational/deploy.json
  echo ""
  sleep 2

  echo "run: bin/transational/01_getProcess.sh"
  ./bin/transational/01_getProcess.sh
  echo ""
  sleep 2

  echo "run: bin/transational/05_RunAsyncRemove.sh"
  ./bin/transational/05_RunAsyncRemove.sh bin/transational/remove.json
  echo ""
  sleep 2

  echo "run: bin/transational/01_getProcess.sh"
  ./bin/transational/01_getProcess.sh


fi


