from gdcmPython import *

### Get filename from command line or default it
try:
   FileName = sys.argv[1]
except IndexError:
   FileName = os.path.join(GDCM_DATA_PATH, "test.acr")

if not os.path.isfile(FileName):
   print "Cannot open file ", FileName
   sys.exit()

toRead = gdcmHeader(FileName)
ValDict = toRead.GetPubEntry()
ExploreElements = ["Patient Name", "Patient ID",
						 "Study Date", "Study Time", "Study ID",
						 "Study Instance UID",
						 "Series Number",
						 "Modality"]

for elem in ExploreElements:
	print "[%s] = " % elem,
	try:
		print "[%s]" % ValDict[elem]
	except KeyError:
		print "NOT FOUND"
		
