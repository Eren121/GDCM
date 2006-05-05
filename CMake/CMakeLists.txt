#-----------------------------------------------------------------------------
# Help outside projects link against gdcm

EXPORT_LIBRARY_DEPENDENCIES(${GDCM_BINARY_DIR}/GDCMLibraryDepends.cmake)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${GDCM_BINARY_DIR}/GDCMBuildSettings.cmake)
#INSTALL_FILES(/lib/gdcm .cmake GDCMBuildSettings)

# For GDCM from build tree:
SET(GDCM_USE_FILE ${GDCM_SOURCE_DIR}/GDCMUse.cmake)
SET(GDCM_INCLUDE_DIRS
  ${GDCM_BINARY_DIR}/     #for gdcmConfigure.h
  ${GDCM_SOURCE_DIR}/src  #for main gdcm include
  )
SET(GDCM_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})
SET(GDCM_LIBRARY_DEPENDS_FILE ${GDCM_BINARY_DIR}/GDCMLibraryDepends.cmake)

# For installed GDCM:
SET(GDCM_USE_FILE ${CMAKE_INSTALL_PREFIX}/lib/GDCM-${GDCM_VERSION}/GDCMUse.cmake)
SET(GDCM_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/GDCM-${GDCM_VERSION})
SET(GDCM_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib/GDCM-${GDCM_VERSION})
SET(GDCM_LIBRARY_DEPENDS_FILE ${CMAKE_INSTALL_PREFIX}/lib/GDCM-${GDCM_VERSION}/GDCMLibraryDepends.cmake)
CONFIGURE_FILE(${GDCM_SOURCE_DIR}/GDCMConfig.cmake.in
               ${GDCM_BINARY_DIR}/GDCMConfig.cmake @ONLY IMMEDIATE)

