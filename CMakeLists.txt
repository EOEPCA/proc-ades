cmake_minimum_required(VERSION 3.5)
project(ades-code-engine-api-framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)

#set(RUN_TESTS OFF)
set(ENV{RUN_TESTS} OFF)
set(ENV{IMPORT_YAML_CPP} OFF)
set(YAML_CPP_BUILD_TESTS OFF: FORCE)

#XML dependences--------------------------------------
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${LIBXSLT_INCLUDE_DIR})
#--------------------------------------------------------

add_subdirectory(3ty/yaml-cpp-0.6.3)
include_directories(3ty/yaml-cpp-0.6.3/include)
include_directories(3ty/nlohmann)

# Jwt Parser
add_subdirectory(3ty/jwt-cpp)
include_directories(3ty/jwt-cpp/include)

add_subdirectory(3ty/proc-comm-lib-ows-1.04)
include_directories(3ty/proc-comm-lib-ows-1.04/includes)


SET(MODS_LIBS z m dl)

set(LIB_SRC src/templates/exec.cpp src/templates/workflow_executor.hpp src/templates/workflow_executor.cpp)
add_library(workflow_executor SHARED ${LIB_SRC})
TARGET_LINK_LIBRARIES(workflow_executor ${MODS_LIBS})

set(LIB_SRC_PEP src/templates/pepresources.hpp src/templates/pepresources.cpp)
add_library(pep_resource SHARED ${LIB_SRC_PEP})
TARGET_LINK_LIBRARIES(pep_resource ${MODS_LIBS})

include_directories(src/templates)
add_executable(pep_test test/mock/pep/app/main.cpp)
TARGET_LINK_LIBRARIES(pep_test ${MODS_LIBS} ${CURL_LIBRARIES})
#cp /work/test/mock/pep/app/main.cpp   /project/test/mock/pep/app/main.cpp  && make  pep_resource   pep_test
#TARGET_LINK_LIBRARIES(ades_main ${MODS_LIBS})

#add_subdirectory(src/deployundeploy/)






