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

### Build the header element list
toRead = gdcmDicomDir(FileName)
if not toRead.IsReadable():
   print "The ", FileName, " DicomDir is not readable with gdcm. Sorry."
   sys.exit()

print "##############################################################"
print "### Display all the elements and their respective values"
print "## found in the ", FileName, " file."
print "##############################################################"
toRead.SetPrintLevel(-1)
toRead.Print()

# ValDict = toRead.GetEntry()
# for key in ValDict.keys():
# 	print "[%s] = [%s]" %(key, ValDict[key])

