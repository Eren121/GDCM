import os, sys

### Setup the path to the dictionaries. WARNING: this needs to be done
#   BEFORE importation of gdcm. We consider we found the dictionaries
#   path when we encounter the standard dictionary i.e. the file dicomV3.dic.
# 1/ first consider the environnement variable.
# 2/ when this fails consider the package installation mode i.e. when the
#    layout is such that the directory containing the dictionaries is
#    BELOW (the root is at the top) the current directory.
# 3/ eventually consider the pre-installation mode i.e. when the
#    layout is such that the directory containing the dictionaries is
#    ABOVE (the root is at the top) the current directory.
#   
try:
	os.environ["GDCM_DICT_PATH"]
	if not os.path.isfile(os.path.join(os.environ["GDCM_DICT_PATH"],
	                                   "dicomV3.dic")):
		raise KeyError
except KeyError:
	# Those pathes have to be relative to the package (hence we use __path__): 
	InstallModePath = os.path.join(__path__[0], "Dicts/")
	if os.path.isfile(os.path.join(InstallModePath, "dicomV3.dic")):
		os.environ["GDCM_DICT_PATH"] = InstallModePath
		PreInstallModePath = None
	else:
		PreInstallModePath = os.path.join(__path__[0], "..", "Dicts/")
		if os.path.isfile(os.path.join(PreInstallModePath, "dicomV3.dic")):
			os.environ["GDCM_DICT_PATH"] = PreInstallModePath
			InstallModePath = None
		else:
			print "Unfound gdcm dictionaries path"
			sys.exit(1)

### Import the swig generated shadow classes.
try:
	from gdcm import *
except ImportError,e:
	print e
	raise ImportError, "gdcm extension not imported."

### Set up the path to the data images (for examples and test suite)
if InstallModePath:
	GDCM_DATA_PATH = os.path.join(__path__[0], "Data")
else:
	if PreInstallModePath:
		GDCM_DATA_PATH = os.path.join(__path__[0], "..", "Data")
	else:
		print "Unfound data path"
		sys.exit(1)

### Get filename from command line or default it
try:
	FileName = sys.argv[1]
except IndexError:
	FileName = os.path.join(GDCM_DATA_PATH, "test.acr")

if not os.path.isfile(FileName):
	print "Cannot open file ", FileName
	sys.exit()
