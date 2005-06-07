# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "21:00:00 EDT")

# Dart server to submit results (used by client)
SET (DROP_METHOD "http")
SET (DROP_SITE "public.kitware.com")
SET (DROP_LOCATION "/cgi-bin/HTTPUploadDartFile.cgi")
SET (TRIGGER_SITE 
    "http://${DROP_SITE}/cgi-bin/Submit-GDCM-TestingResults.cgi")

# Dart server configuration 
SET (CVS_WEB_URL "http://www.creatis.insa-lyon.fr/viewcvs/viewcvs.cgi/gdcm/")
SET (CVS_WEB_CVSROOT "")
