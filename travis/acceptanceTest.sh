#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh


if [ "${TRAVIS}" == "true" ]; then
  docker pull "${EOEPCA_ADES_ZOO}"


  sudo docker run --rm  -d --name zoo -p 7777:80 ${EOEPCA_ADES_ZOO}

  sleep 15

  chmod +x ./travis/transational/0*
  chmod +x ./travis/transational/wps1_getProcess*

  echo "run: ./travis/transational/01_getProcess.sh"
  ./travis/transational/01_getProcess.sh
  echo ""
  sleep 2

  echo "run: ./travis/transational/01_getProcessRdirienzo.sh"
  ./travis/transational/01_getProcessRdirienzo.sh
  echo ""
  sleep 2

  echo "show working area"
  docker exec zoo find /opt/zooservices_user

  echo "run: bin/transational/02_RunAsync.sh"
  ./travis/transational/02_RunAsync.sh travis/transational/deploy.json
  echo ""
  sleep 2

  echo "run: travis/transational/02_RunSyncRdirienzo.sh"
  ./travis/transational/02_RunSyncRdirienzo.sh travis/transational/deploy.json
  echo ""
  sleep 2

  echo "show rdirienzo's working area"
  docker exec zoo find /opt/zooservices_user/rdirienzo

  echo "run: bin/transational/01_getProcess.sh"
  ./travis/transational/01_getProcess.sh
  echo ""
  sleep 2

  echo "run: travis/transational/05_RunAsyncRemoveRdirienzo.sh"
  ./travis/transational/05_RunAsyncRemoveRdirienzo.sh travis/transational/remove.json
  echo ""
  sleep 2

  echo "run: ./travis/transational/01_getProcessRdirienzo.sh"
  ./travis/transational/01_getProcessRdirienzo.sh
  echo ""
  sleep 2

  echo "show rdirienzo's working area "
  docker exec zoo find /opt/zooservices_user/rdirienzo

  echo "catalog CWL referrer: https://catalog.terradue.com/eoepca-services/search?uid=test_entites"
  curl -s "https://catalog.terradue.com/eoepca-services/search?uid=test_entites"

  echo "install application"
  ./travis/transational/02_RunSyncRdirienzo.sh travis/transational/deploy_test.json

  sleep 10
  echo "new test service"
  ./travis/transational/01_getProcessRdirienzo.sh "test_entites_" 

  sleep 10
  echo "md5sum..."
  mSum=$(./travis/transational/01_getProcessRdirienzo.sh "test_entites_"  | md5sum)

  echo "md5sum: $mSum"
  echo "$mSum" | grep "3f317eb9e0da77f5c3b217482250e55b"
  if [ $? -eq 0 ]
  then
    echo "OK"
  else
    echo "MD5SUM failed"
    exit 1
  fi

fi


