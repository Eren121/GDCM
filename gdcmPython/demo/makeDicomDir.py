from gdcmPython import *
import sys

### Get filename from command line or default it
try:
   FileName = sys.argv[1]
except IndexError:
   FileName = os.path.join(GDCM_DATA_PATH, "DICOMDIR")

try:
   printLevel = int(sys.argv[2])
except IndexError:
   printLevel = 1
print FileName

### Build the header element list
toRead = gdcmDicomDir(FileName)

def startMethod():
	print "Python start"

def endMethod():
	print "Python end"

def progressMethod():
	print "Python progress",toRead.GetProgress()

toRead.SetStartMethod(None)
toRead.SetProgressMethod(progressMethod)
toRead.SetEndMethod(endMethod)
toRead.ParseDirectory()

print "##############################################################"
print "### Display all the elements and their respective values"
print "## found in the ", FileName, " file."
print "##############################################################"
toRead.SetPrintLevel(-1)
toRead.Print()

ValDict = toRead.GetEntry()
for key in ValDict.keys():
	print "[%s] = [%s]" %(key, ValDict[key])

