# Import GDCM's build settings.

INCLUDE(
  ${CMAKE_ROOT}/Modules/CMake/CMAkeImportBuildSettings.cmake
  )

CMAKE_IMPORT_BUILD_SETTINGS(${GDCM_BUILD_SETTINGS_FILE})

# Tell the compiler where to find GDCM's header files.
INCLUDE_DIRECTORIES(${GDCM_INCLUDE_DIRS})

# Tell the linker where to find GDCM's libraries.
LINK_DIRECTORIES(${GDCM_LIBRARY_DIRS})
