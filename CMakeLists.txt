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



add_subdirectory(3ty/proc-comm-lib-ows-1.03)
include_directories(3ty/proc-comm-lib-ows-1.03/includes)


SET(MODS_LIBS z m dl)

set(LIB_SRC src/templates/workflow_executor.hpp src/templates/workflow_executor.cpp)
add_library(workflow_executor SHARED ${LIB_SRC})
TARGET_LINK_LIBRARIES(workflow_executor ${MODS_LIBS})

#add_executable(ades_main ${src_main})
#TARGET_LINK_LIBRARIES(ades_main ${MODS_LIBS})

#add_subdirectory(src/deployundeploy/)






