#!/usr/bin/env bash

RELEASETYPE=''


export SERVICE_NAME="template-service-cpp"
export LOCAL_SERVICE_NAME="local_${SERVICE_NAME}"

#null definition
export NULL='none'

#internal DokerImage name
export LOCAL_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'

#get branch name
TRAVIS_BRANCH="${TRAVIS_BRANCH:-develop}"

#change name for branch feature ex feature/EOEPCA-38 change in  feature_EOEPCA_38
TRAVIS_BRANCH=$(echo ${TRAVIS_BRANCH}| sed -e 's@/@_@g')
TRAVIS_BRANCH=$(echo ${TRAVIS_BRANCH}| sed -e 's@-@_@g')

export TRAVIS_BRANCH
export CMAKERELEASE="Debug"

#simple anchor
if [ "${TRAVIS_BRANCH}" == 'master' ]
then
  CMAKERELEASE="Release"
  RELEASETYPE='release'
	echo 'Branch selected: master '
fi

#simple anchor
if [ "${TRAVIS_BRANCH}" == 'develop' ]
then
  RELEASETYPE='develop'
	echo 'Branch selected: develop' 
fi


