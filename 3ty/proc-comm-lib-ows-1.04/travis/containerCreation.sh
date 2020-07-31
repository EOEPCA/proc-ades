#!/usr/bin/env bash

# fail fast settings from https://dougrichardson.org/2018/08/03/fail-fast-bash-scripting.html
set -euov pipefail


source travis/variables.sh

source travis/libs/build.sh

if [ "${TRAVIS}" == "true" ]
then
  docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} ./test/eoepca-test
fi


docker build -t ${EOEPCA_REPOSITORY}/${LOCAL_SERVICE_NAME} .

docker tag ${EOEPCA_REPOSITORY}/${LOCAL_SERVICE_NAME} ${EOEPCA_REPOSITORY}/${SERVICE_NAME}:$buildTag

echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin

docker push ${EOEPCA_REPOSITORY}/${SERVICE_NAME}:$buildTag