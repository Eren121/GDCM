# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "1:00:00 EDT")

# Dart server to submit results (used by client)
SET (DROP_SITE "public.kitware.com")
SET (DROP_LOCATION "/incoming")
SET (DROP_SITE_USER "ftpuser")
SET (DROP_SITE_PASSWORD "public")
SET (DROP_METHOD "ftp")
SET (TRIGGER_SITE 
       "http://${DROP_SITE}/cgi-bin/Submit-Public-TestingResults.pl")

# Project Home Page
SET (PROJECT_URL "http://www.creatis.insa-lyon.fr/Public/Gdcm")

# Dart server configuration 
SET (ROLLUP_URL "http://${DROP_SITE}/cgi-bin/Public-rollup-dashboard.sh")
SET (CVS_WEB_URL "http://${DROP_SITE}/cgi-bin/cvsweb.cgi/Public/")
SET (CVS_WEB_CVSROOT "Public")

OPTION(BUILD_DOXYGEN "Build source documentation using doxygen" "Off")
SET (DOXYGEN_CONFIG "${PROJECT_BINARY_DIR}/doxygen.config" )
SET (USE_DOXYGEN "On")
SET (DOXYGEN_URL "http://${DROP_SITE}/Public/Doxygen/html/" )

SET (USE_GNATS "On")
SET (GNATS_WEB_URL "http://${DROP_SITE}/Bug/index.php")

# Continuous email delivery variables
SET (CONTINUOUS_FROM "luis.ibanez@kitware.com")
SET (SMTP_MAILHOST "public.kitware.com")
SET (CONTINUOUS_MONITOR_LIST "luis.ibanez@kitware.com")
SET (CONTINUOUS_BASE_URL "http://www.kitware.com/Testing")

MARK_AS_ADVANCED(BUILD_DOXYGEN)

