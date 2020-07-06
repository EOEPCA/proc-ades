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

	curl -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi

	curl -s -L -X GET "http://localhost:7777/wps3/" -H  "accept: application/json"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi

	curl -s -L -X GET "http://localhost:7777/wps3/processes" -H  "accept: application/json"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi

	curl -s -L -X GET "http://localhost:7777/wps3/processes/GetStatus" -H  "accept: application/json"
	if [ "$?" -ne 0 ]
	then
		echo "curl test failed"
		exit 1
	fi


  echo "run longProcess  test"

  curl -s -D /tmp/header  -v -L -X POST "http://localhost:7777/wps3/processes/longProcess/jobs" -H  "accept: application/json" -H  "Prefer: respond-async" -H  "Content-Type: application/json" -d "{\"inputs\":[{\"id\":\"string\"}],\"outputs\":[{\"format\":{\"mimeType\":\"string\",\"schema\":\"string\",\"encoding\":\"string\"},\"id\":\"Result\",\"transmissionMode\":\"value\"}]}"
  job=$(grep "Location: " /tmp/header  | cut -d' ' -f2 | tr -d '\r')
  echo "run jobId: ${job}"

  echo "get Status:"
  curl -s -L "http://localhost:7777/${job}" -H "accept: application/json"

  echo  "Sleep 1s"
  sleep 1

  echo "get Status:"
  curl -s -L "http://localhost:7777/${job}" -H "accept: application/json"

  echo  "Sleep 20s"
  sleep 20

  echo "get Status:"
  curl -s -L "http://localhost:7777/${job}" -H "accept: application/json"

  echo "get result:"
  curl -s -L "http://localhost:7777/${job}/result" -H "accept: application/json"

fi

