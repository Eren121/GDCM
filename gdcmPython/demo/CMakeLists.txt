# Python testing directory, 'demo' is not very explicit ...

#-----------------------------------------------------------------------------
# DOH! python is not found by default
FIND_PROGRAM(PYTHON_EXECUTABLE
    NAMES python2.4 python2.3 python2.2 python2.1 python2.0 python1.6 python1.5 python
    PATHS
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.4\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]
)
MARK_AS_ADVANCED(PYTHON_EXECUTABLE)

IF(PYTHON_EXECUTABLE)
  ADD_TEST(Python-PrintHeader ${PYTHON_EXECUTABLE}
    ${GDCM_BINARY_DIR}/gdcmPython/demo/PrintHeader.py
    )
ELSE(PYTHON_EXECUTABLE)
  MESSAGE(FATAL_ERROR "You requested testing for python, but python was not
    found")
ENDIF(PYTHON_EXECUTABLE)

#-----------------------------------------------------------------------------
# Copy all the demo directory content to the cmake bin 
# Without it, tests can't be launched
CONFIGURE_FILE(
    ${GDCM_SOURCE_DIR}/gdcmPython/demo/PrintHeader.py.in
    ${GDCM_BINARY_DIR}/gdcmPython/demo/PrintHeader.py
)

