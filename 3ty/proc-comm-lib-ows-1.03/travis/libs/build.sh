#!/usr/bin/env bash

if [ "${BUILDER_ENV_IMAGE}" == "${NULL}" ] #image is not defined
then
	if [ "${BUILDER_ENV_IMAGE_NEW_TAG}" == "${NULL}" ]
	then
	  # import build script
		source travis/libs/builder.sh
	else
    #pull the user's image
		docker pull ${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${BUILDER_ENV_IMAGE_NEW_TAG}
		if [ $? -ne 0 ] #pull failed
		then
		  #pull failed, build the builder image
			source travis/libs/builder.sh
		else
			LOCAL_DOCKERIMAGE=${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${BUILDER_ENV_IMAGE_NEW_TAG}
		fi
	fi

else
  #set the user's image
	LOCAL_DOCKERIMAGE=${BUILDER_ENV_IMAGE}
fi

echo "Docker image: ${LOCAL_DOCKERIMAGE}"




