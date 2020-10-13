#!/usr/bin/env bash

#rm -fvR build zooservice

if [ -z "${BUILDER_DOCKERIMAGE}" ]; then
  BUILDER_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'
fi

if [ -z "${CMAKERELEASE}" ]; then
  CMAKERELEASE='Release'
fi

if [ -z "${LOCAL_IMAGE_NAME}" ]; then
  LOCAL_IMAGE_NAME='proc-ades:build'
fi

HERE=$PWD
cd 3ty/proc-comm-zoo-1.2-alpha
export addUserFeature="True"
chmod +x ./scripts/build.sh
./scripts/build.sh
if [ $? -ne 0 ]; then
#  echo "3ty/proc-comm-zoo-1.2-alpha/scripts/build.sh  ${LOCAL_IMAGE_NAME} failed"
  echo "RDR ERROR"
  exit 2
fi


cd $HERE
docker build --rm -t ${LOCAL_IMAGE_NAME} .
if [ $? -ne 0 ]; then
  echo "docker build --rm -t ${LOCAL_IMAGE_NAME} failed"
  exit 2
fi

docker tag proc-ades:build eoepca/proc-ades:latest

