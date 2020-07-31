#!/usr/bin/env bash

# fail fast settings from https://dougrichardson.org/2018/08/03/fail-fast-bash-scripting.html
#set -euov pipefail

#import globals
source travis/variables.sh

if [ "${BUILDER_ENV_IMAGE}" == "${NULL}" -a "${BUILDER_ENV_IMAGE_NEW_TAG}" != "${NULL}" ]
then
  # import build script
	source travis/libs/builder.sh
fi




