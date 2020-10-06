#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh

export BUILD_ONLY='true'
scripts/build.sh

if [ "${TRAVIS}" == "true" ]; then

  docker tag ${LOCAL_IMAGE_NAME} "${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${TRAVIS_NAME}_${TRAVIS_BUILD_NUMBER}"
  docker tag ${LOCAL_IMAGE_NAME} "${EOEPCA_ADES_ZOO}"
  if [ "$?" -ne 0 ]; then
    echo "docker tag ${LOCAL_IMAGE_NAME}:${buildTag} --> ${EOEPCA_DOCKERIMAGE}:${buildTag} failed"
    exit 1
  fi

  echo "${DOCKER_PASSWORD}" | docker login -u "${DOCKER_USERNAME}" --password-stdin
  if [ "$?" -ne 0 ]; then
    echo "Docker Hub login failed"
    exit 1
  fi

  docker push "${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${TRAVIS_NAME}_${TRAVIS_BUILD_NUMBER}"
  docker push "${EOEPCA_ADES_ZOO}"
  if [ "$?" -ne 0 ]; then
    echo "Docker push ${EOEPCA_ADES_ZOO} failed"
    exit 1
  fi

fi
