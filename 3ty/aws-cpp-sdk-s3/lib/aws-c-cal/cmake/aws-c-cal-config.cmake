include(CMakeFindDependencyMacro)

find_dependency(aws-c-common)

if (NOT BYO_CRYPTO AND NOT WIN32 AND NOT APPLE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")
    find_dependency(LibCrypto)
endif()

if (BUILD_SHARED_LIBS)
    include(${CMAKE_CURRENT_LIST_DIR}/shared/aws-c-cal-targets.cmake)
else()
    include(${CMAKE_CURRENT_LIST_DIR}/static/aws-c-cal-targets.cmake)
endif()

