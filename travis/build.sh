#!/usr/bin/env bash

set -euov pipefail

source travis/libs/variables.sh

if [ "${TRAVIS}" != "true" ]
then
	export BUILD_ONLY='true'
	scripts/build.sh
fi



