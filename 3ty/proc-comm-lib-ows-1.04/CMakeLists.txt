cmake_minimum_required(VERSION 3.5)
project(eoepcaows)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)
set(CMAKE_CXX_FLAGS "-fPIC")

set(RUNNER_NAME
		runner
)

set(CATALOG_RUNNER_NAME
		catalogrunner
		)


#XML dependences--------------------------------------
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${LIBXSLT_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

#--------------------------------------------------------
include_directories(includes)

# YAML-CPP
if((NOT DEFINED ENV{IMPORT_YAML_CPP}) OR ($ENV{IMPORT_YAML_CPP}))
	execute_process(COMMAND ${CMAKE_SOURCE_DIR}/bin/download3ty.sh
			RESULT_VARIABLE result
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

	set(YAML_CPP_BUILD_TESTS OFF: FORCE)
	add_subdirectory(3ty/yaml-cpp-yaml-cpp-0.6.3)
	include_directories(3ty/yaml-cpp-yaml-cpp-0.6.3/include)
ENDIF()

SET(MODS_LIBS
		z m dl
		)

SET(INCLUDE_SRC
		includes/eoepca/IMod.hpp
		includes/eoepca/owl/eoepcaows.hpp includes/eoepca/owl/owsparameter.hpp)

SET(LIB_INCLUDE
	lib/includes/yaml/yaml.hpp
	lib/includes/yaml/yamlutils.hpp
	lib/includes/yaml/object.hpp
)

SET(LIB_SRC
		lib/eoepcaows.cpp
		lib/parser.cpp lib/includes/parser.hpp includes/eoepca/owl/eoepcaows.hpp
		)

SET(MAIN_SRC
	src/run.cpp
)

SET(CATALOG_SRC
		src/catalog.cpp
		)


add_library(eoepcaows SHARED ${LIB_SRC} ${LIB_INCLUDE} )
TARGET_LINK_LIBRARIES(eoepcaows yaml-cpp ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${MODS_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})

add_executable(${RUNNER_NAME} ${INCLUDE_SRC} ${MAIN_SRC})
TARGET_LINK_LIBRARIES(${RUNNER_NAME} ${MODS_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})

add_executable(${CATALOG_RUNNER_NAME} ${INCLUDE_SRC} ${CATALOG_SRC})
TARGET_LINK_LIBRARIES(${CATALOG_RUNNER_NAME} ${MODS_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES}  ${CURL_LIBRARIES} )

if((NOT DEFINED ENV{RUN_TESTS}) OR ($ENV{RUN_TESTS}))
add_subdirectory(tests)
ENDIF()

#/Users/rdirienzo/Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake