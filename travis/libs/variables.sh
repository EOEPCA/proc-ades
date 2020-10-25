#!/usr/bin/env bash

export RELEASETYPE=''

#null definition
export NULL='none'

#get branch name
TRAVIS_BRANCH="${TRAVIS_BRANCH:-develop}"

#change name for branch feature ex feature/EOEPCA-38 change in  feature_EOEPCA_38
TRAVIS_BRANCH=$(echo ${TRAVIS_BRANCH}| sed -e 's@/@_@g')
TRAVIS_BRANCH=$(echo ${TRAVIS_BRANCH}| sed -e 's@-@_@g')

export TRAVIS_BRANCH

TRAVIS_NAME="travis_"

export CMAKERELEASE="Debug"

export TAG_PREFIX=''

#simple anchor
if [ "${TRAVIS_BRANCH}" == 'master' ]
then
  CMAKERELEASE="Release"
  RELEASETYPE='release'
  TRAVIS_NAME=''
  TAG_PREFIX=''
	echo 'Branch selected: master '
fi

#simple anchor
if [ "${TRAVIS_BRANCH}" == 'develop' ]
then
  RELEASETYPE='develop'
  TAG_PREFIX='dev'
	echo 'Branch selected: develop' 
fi

#docker requirements
export BUILDER_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'
export DOCKER_ZOO='eoepca/proc-comm-zoo:latest'

#new definitions
export TRAVIS_BUILD_NUMBER="${TRAVIS_BUILD_NUMBER:-0}"

#docker temp image name
export LOCAL_IMAGE_NAME="proc-ades:build"

#eoepca repository
export EOEPCA_REPOSITORY='eoepca'
#eoepca name
export EOEPCA_IMAGE="proc-ades"
#eoepca tag
export EOEPCA_TAG="${TAG_PREFIX}latest"
#final image
export EOEPCA_ADES_ZOO="${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${EOEPCA_TAG}"



