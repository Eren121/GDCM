from gdcmPython.core import *
import sys
import os

### Get filename from command line or default it
try:
   fileName = sys.argv[1]
except IndexError:
   fileName = os.path.join(GDCM_DATA_PATH, "test.acr")

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
print fileName, type(fileName)
header = gdcm.Header(fileName)
if not header.IsReadable():
   print "The '", fileName, "' file is not readable with gdcm. Sorry."
   sys.exit()

print "##############################################################"
print "### Display all the elements and their respective values"
print "## found in the ", fileName, " file."
print "##############################################################"

header.InitTraversal()
val=header.GetNextEntry()
while(val):
	val.Print()
	print ""
	val=header.GetNextEntry()
val=None

