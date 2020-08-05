##!/usr/bin/env bash
#
##rm -fvR build zooservice
#
#if [ -z "${BUILDER_DOCKERIMAGE}" ]; then
#  BUILDER_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'
#fi
#
#if [ -z "${CMAKERELEASE}" ]; then
#  CMAKERELEASE='Release'
#fi
#
#if [ -z "${LOCAL_IMAGE_NAME}" ]; then
#  LOCAL_IMAGE_NAME='proc-ades:1.0'
#fi
#
#mkdir -p build
#
#docker run --rm -ti -v $PWD:/project/ -w /project/build/ ${BUILDER_DOCKERIMAGE} cmake -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" ..
#if [ $? -ne 0 ]; then
#  echo "CMAKE release ${CMAKERELEASE} failed"
#  exit 2
#fi
#
##eoepcaargo sources ades_main argo_interface
#docker run --rm -ti -v $PWD:/project/ -w /project/build/ ${BUILDER_DOCKERIMAGE} make eoepcaows workflow_executor
#if [ $? -ne 0 ]; then
#  echo "make eoepcaargo failed"
#  exit 2
#fi
##cp /project/build/libworkflow_executor.so /opt/t2service/libworkflow_executor.so
#
HERE=$PWD
cd 3ty/proc-comm-zoo-1.2-alpha
export addUserFeature="True"
chmod +x ./scripts/build.sh
./scripts/build.sh
if [ $? -ne 0 ]; then
#  echo "3ty/proc-comm-zoo-1.2-alpha/scripts/build.sh  ${LOCAL_IMAGE_NAME} failed"
  echo "RDR ERROR"
  exit 2
fi
#
#cd $HERE
#docker run --rm -w /project/zooservice -v $PWD:/project proc-comm-zoo:1.0 make -C ../src/deployundeploy/zoo/
#if [ $? -ne 0 ]; then
#  echo "make deployundeploy failed"
#  exit 2
#fi
#
#docker run --rm -w /project/zooservice -v $PWD:/project proc-comm-zoo:1.0 make -C ../src/templates interface
#if [ $? -ne 0 ]; then
#  echo "make libInterface failed"
#  exit 2
#fi
#
#docker build --rm -t ${LOCAL_IMAGE_NAME} .
#if [ $? -ne 0 ]; then
#  echo "docker build --rm -t ${LOCAL_IMAGE_NAME} failed"
#  exit 2
#fi
#
#
