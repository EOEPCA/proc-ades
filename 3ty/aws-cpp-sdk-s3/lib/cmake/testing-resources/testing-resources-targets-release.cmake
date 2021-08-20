#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "testing-resources" for configuration "Release"
set_property(TARGET testing-resources APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(testing-resources PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtesting-resources.so"
  IMPORTED_SONAME_RELEASE "libtesting-resources.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS testing-resources )
list(APPEND _IMPORT_CHECK_FILES_FOR_testing-resources "${_IMPORT_PREFIX}/lib/libtesting-resources.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
