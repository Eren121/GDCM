from gdcmPython import *
toRead = gdcmHeader(FileName)
toRead.LoadElements()

print "##############################################################"
print "### Display all the elements and their respective values"
print "## found in the ", FileName, " file."
print "##############################################################"
ValDict = toRead.GetPubElVal()
for key in ValDict.keys():
	print "[%s] = [%s]" %(key, ValDict[key])
