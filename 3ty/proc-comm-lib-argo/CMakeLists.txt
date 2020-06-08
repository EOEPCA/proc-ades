cmake_minimum_required(VERSION 3.5)


configure_file(CMakeLists.txt.in proc-comm-lib-argo-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/proc-comm-lib-argo-download )

if(result)
    message(FATAL_ERROR "CMake step for owslib failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/proc-comm-lib-argo-download )
if(result)
    message(FATAL_ERROR "Build step for owslib failed: ${result}")
endif()

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/proc-comm-lib-argo
        ${CMAKE_CURRENT_BINARY_DIR}/proc-comm-lib-argo-build
        EXCLUDE_FROM_ALL)



#SET(PROC_COMM_ZOO_SVC_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/3ty/proc-comm-lib-ows/proc-comm-lib-ows/includes  PARENT_SCOPE)








