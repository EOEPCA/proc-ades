#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh

if [ "${TRAVIS}" == "true" ]
then

	echo ""

	docker pull "${EOEPCA_DOCKERIMAGE}:${buildTag}"
	if [ "$?" -ne 0 ]
	then
		echo "Docker pull ${EOEPCA_DOCKERIMAGE}:${buildTag} failed"
		exit 1
	fi

	sudo docker run -d --rm -p 7777:80 ${EOEPCA_DOCKERIMAGE}:${buildTag}

	sleep 15 # wait until the container is running

	curl -L  "http://localhost:7777/rdirienzo/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi

	curl -s -L -X GET "http://localhost:7777/rdirienzo/wps3/" -H  "accept: application/json"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi

	curl -s -L -X GET "http://localhost:7777/rdirienzo/wps3/processes" -H  "accept: application/json"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi

	curl -s -L -X GET "http://localhost:7777/rdirienzo/wps3/processes/GetStatus" -H  "accept: application/json"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi
	
fi

