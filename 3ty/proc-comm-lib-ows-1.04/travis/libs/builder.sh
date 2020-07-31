#!/usr/bin/env bash

BUILDIMAGE="Build"
if [ "${BUILDER_ENV_IMAGE}" != "${NULL}" ] #is defined a public DockerImageCompiler
then
	docker pull ${BUILDER_ENV_IMAGE}
	if [ $? -ne 0 ] #pull failed, build my DockerImageCompiler
	then
		BUILDIMAGE="Build"
	else
		BUILDIMAGE="${NULL}"
		docker tag ${BUILDER_ENV_IMAGE} ${LOCAL_DOCKERIMAGE}
	fi
fi

# the Image must be buildt
if [ ${BUILDIMAGE} != "${NULL}" ]
then
	docker build --rm --no-cache -t ${LOCAL_DOCKERIMAGE} -f Dockerfile_builder .

fi

docker tag ${LOCAL_DOCKERIMAGE} ${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:$buildTag

# check if the Image must be push
if [ "${BUILDER_ENV_IMAGE_NEW_TAG}" != "${NULL}" ]
then
	echo "${DOCKER_PASSWORD}" | docker login -u "${DOCKER_USERNAME}" --password-stdin
	docker tag ${LOCAL_DOCKERIMAGE} "${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${BUILDER_ENV_IMAGE_NEW_TAG}"
	echo "Image ${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${BUILDER_ENV_IMAGE_NEW_TAG} created."

	docker push "${EOEPCA_REPOSITORY}/${EOEPCA_IMAGE}:${BUILDER_ENV_IMAGE_NEW_TAG}"
fi



