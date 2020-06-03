
cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()


file(GLOB test_sources [a-z_]*.cpp application/[a-z_]*.hpp application/[a-z_]*.cpp)

SET(MODS_LIBS
        z m dl
        )

SET(LIB_TEST_NAME
    libtest-test
)

add_executable(${LIB_TEST_NAME} ${test_sources})
target_link_libraries(${LIB_TEST_NAME} ${MODS_LIBS} gtest_main)
add_test(NAME eoepca_test_run COMMAND ${LIB_TEST_NAME})

