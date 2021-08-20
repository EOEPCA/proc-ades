
cmake_minimum_required(VERSION 2.8)
project(zooServiceBuild)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)
set(CMAKE_CXX_FLAGS "-fPIC")

#XML dependences--------------------------------------

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${LIBXSLT_INCLUDE_DIR})
#--------------------------------------------------------

# nlohmann
include_directories(3ty/nlohmann)
# Jwt Parser
add_subdirectory(3ty/jwt-cpp)
include_directories(3ty/jwt-cpp/include)

# aws-cpp-sdk-s3
list(APPEND CMAKE_PREFIX_PATH "3ty/aws-cpp-sdk-s3")
find_package(AWSSDK REQUIRED COMPONENTS s3)

#${LIBXML2_LIBRARIES}

include_directories(includes)
include_directories(/var/lib64 /usr/local/include/t2wpsZoolib /opt/t2build/includes/ /opt/zoo-project/zoo-project/zoo-kernel/ . ./include )
include_directories(3ty/proc-comm-lib-ows-1.04/includes)

SET(LIB_SRC
        src/deployundeploy/zoo/deploy.cpp
        src/deployundeploy/zoo/xmlmemorywritewrapper.cpp
        )

SET(MODS_LIBS z m dl)


add_library(epcatransactional SHARED ${LIB_SRC})
TARGET_LINK_LIBRARIES(epcatransactional /work/build/libzooconverter.a ${CURL_LIBRARIES} zoo_service ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${MODS_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES})

install(TARGETS epcatransactional DESTINATION /work/zooservices/)






