from load import *

########## Real usage of Pygdcm starts here
toRead = gdcm.gdcmHeader(FileName)
toRead.LoadElements()
ValDict = toRead.GetPubElVal()
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
