#!/usr/bin/env bash

#rm -fvR build demo
mkdir -p "build"

export BUILDER_ENV_IMAGE="eoepca/eoepca-build-cpp:1.0"

if [ ! -z "${LOCAL_DOCKERIMAGE}" ]; then
  BUILDER_ENV_IMAGE=${LOCAL_DOCKERIMAGE}
fi

if [ ! -z "${CMAKERELEASE}" ]; then
  export CMAKERELEASE=Debug
fi

docker run --rm -ti -v $PWD:/project/ -w /project/build ${BUILDER_ENV_IMAGE} cmake -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" build ..

docker run --rm -ti -v $PWD:/project/ -w /project/build ${BUILDER_ENV_IMAGE} make eoepcaows runner libtest-test

mkdir -p demo
cp $PWD/tests/application/assets/ows2.xml $PWD/build/libeoepcaows.so $PWD/build/runner $PWD/build/tests/libtest-test demo

cd demo && ./runner ows2.xml && ./libtest-test --gtest_break_on_failure
