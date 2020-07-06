#!/usr/bin/env bash

if [ -z "${LOCAL_SERVICE_NAME}" ]
then
	LOCAL_SERVICE_NAME="proc-comm-zoo"
fi

if [ -z "${buildTag}" ]
then
	buildTag="1.0"
fi


if [ -n "${addUserFeature}" ]
then
	WITH_USER='--build-arg addUserFeature=TRUE '
fi

echo "docker build --rm -t ${LOCAL_SERVICE_NAME}:${buildTag} ${WITH_USER} ."
docker build --rm -t ${LOCAL_SERVICE_NAME}:${buildTag} ${WITH_USER} .
if [ $? -ne 0 ]
then
	echo "build Docker image  failed"
	exit 3
fi


