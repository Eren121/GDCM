import os, sys

### Defaulting the path to the dictionaries
# WARNING: this needs to be done before importation of gdcm !
# FIXME: this needs to be put in a wrapper of the swig generated
#			shadow classes (say Pygdcm.py?)
try:
	os.environ["GDCM_DICT_PATH"]
except KeyError:
	os.environ["GDCM_DICT_PATH"]="../../Dicts/"

### When not properly installed (like in a cvs hierachy) try
#	 to handle properly the import of gdcm
try:
	import gdcm
except ImportError:
	import sys, os
	PreInstallPath = os.path.join(os.getcwd(), "..")
	sys.path.append(PreInstallPath)
	try:
		import gdcm
	except ImportError,e:
		raise ImportError, "gdcm extension module not found"
		sys.exit()

### Get filename from command line or default it
try:
	FileName = sys.argv[1]
except IndexError:
	FileName = os.path.join("..", "..", "Test", "test.acr")
if not os.path.isfile(FileName):
	print "Cannot open file ", FileName
	sys.exit()
