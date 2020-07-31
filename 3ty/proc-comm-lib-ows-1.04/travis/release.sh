#!/usr/bin/env bash

#import globals
source travis/variables.sh

if [ "${TRAVIS}" != "true" ]
then

	cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" -S . -B build/
	if [ $? -ne 0 ]
	then
		echo "CMAKE prepare failed"
		exit
	fi

	cmake --build build/  --target all -- -j 2
  if [ $? -ne 0 ]
  then
    echo "Build failed"
		exit
  fi


else

  source travis/libs/build.sh

  source scripts/build.sh

fi