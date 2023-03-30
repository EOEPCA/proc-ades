#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh

#run script
scripts/build.sh

if [ "${TRAVIS}" == "true" ]
then #send push
	
	docker tag ${LOCAL_SERVICE_NAME}:${buildTag} ${EOEPCA_DOCKERIMAGE}:${buildTag}
	if [ "$?" -ne 0 ]
	then
		echo "docker tag ${LOCAL_DOCKERIMAGE}:${buildTag} --> ${EOEPCA_DOCKERIMAGE}:${buildTag} failed"
		exit 1
	fi

	echo "${DOCKER_PASSWORD}" | docker login -u "${DOCKER_USERNAME}" --password-stdin
	if [ "$?" -ne 0 ]
	then
		echo "Docker Hub login failed"
		exit 1
	fi

	docker push "${EOEPCA_DOCKERIMAGE}:${buildTag}"
	if [ "$?" -ne 0 ]
	then
		echo "Docker push ${EOEPCA_DOCKERIMAGE}:${buildTag} failed"
		exit 1
	fi
fi

