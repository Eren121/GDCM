from gdcmPython import *
import sys
import os

### Get filename from command line or default it
try:
   FileName = sys.argv[1]
except IndexError:
   FileName = os.path.join(GDCM_DATA_PATH, "test.acr")

try:
   printLevel = int(sys.argv[2])
except IndexError:
   printLevel = 1

#if not os.path.isfile(FileName):
#   print "Cannot open file ", FileName
#   sys.exit()

# On debugging purposes uncomment the next line
#s = raw_input("Hit any key in this window to exit")

### Build the header element list
print FileName, type(FileName)
toRead = gdcm.Header(FileName)
if not toRead.IsReadable():
   print "The ", FileName, " file is not readable with gdcm. Sorry."
   sys.exit()

print "##############################################################"
print "### Display all the elements and their respective values"
print "## found in the ", FileName, " file."
print "##############################################################"
###toRead.SetPrintLevel(printLevel)
###toRead.Print()

ValDict = toRead.GetEntry()
for key in ValDict.keys():
 	print "[%s] = [%s]" %(key, ValDict[key])

