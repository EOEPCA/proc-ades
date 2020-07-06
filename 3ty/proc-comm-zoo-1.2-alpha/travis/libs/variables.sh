#!/usr/bin/env bash

RELEASETYPE=''

export NULL='none'

export SERVICE_NAME="proc-comm-zoo"
export LOCAL_SERVICE_NAME="local_${SERVICE_NAME}"

#eoepca repository
export EOEPCA_REPOSITORY='eoepca'

#internal DokerImage name
export EOEPCA_DOCKERIMAGE="${EOEPCA_REPOSITORY}/${SERVICE_NAME}"

#get branch name
TRAVIS_BRANCH="${TRAVIS_BRANCH:-develop}"

#change name for branch feature ex feature/EOEPCA-38 change in  feature_EOEPCA_38
TRAVIS_BRANCH=$(echo ${TRAVIS_BRANCH}| sed -e 's@/@_@g')
TRAVIS_BRANCH=$(echo ${TRAVIS_BRANCH}| sed -e 's@-@_@g')


TRAVIS_NAME="travis_"

export TRAVIS_BRANCH
export TRAVIS_NAME

#simple anchor
if [ "${TRAVIS_BRANCH}" == 'master' ]
then
	echo 'Branch selected: master '
	TRAVIS_NAME=''
fi

#simple anchor
if [ "${TRAVIS_BRANCH}" == 'develop' ]
then
	echo 'Branch selected: develop' 
fi

#new definitions


export TRAVIS_BUILD_NUMBER="${TRAVIS_BUILD_NUMBER:-0}"
export buildTag=${TRAVIS_BRANCH}_${TRAVIS_BUILD_NUMBER}

