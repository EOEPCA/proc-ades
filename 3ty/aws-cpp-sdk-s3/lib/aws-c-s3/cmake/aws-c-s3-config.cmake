include(CMakeFindDependencyMacro)

find_dependency(aws-c-auth)
find_dependency(aws-c-http)

if (BUILD_SHARED_LIBS)
    include(${CMAKE_CURRENT_LIST_DIR}/shared/aws-c-s3-targets.cmake)
else()
    include(${CMAKE_CURRENT_LIST_DIR}/static/aws-c-s3-targets.cmake)
endif()
