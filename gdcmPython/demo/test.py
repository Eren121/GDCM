from gdcmPython import *

### Get filename from command line or default it
try:
   FileName = sys.argv[1]
except IndexError:
   FileName = os.path.join(GDCM_DATA_PATH, "test.acr")

if not os.path.isfile(FileName):
   print "Cannot open file ", FileName
   sys.exit()

# On debugging purposes uncomment the next line
#s = raw_input("Hit any key in this window to exit")

### Build the header element list
toRead = gdcmHeader(FileName)
toRead.LoadElements()

print "##############################################################"
print "### Display all the elements and their respective values"
print "## found in the ", FileName, " file."
print "##############################################################"
ValDict = toRead.GetPubElVal()
for key in ValDict.keys():
	print "[%s] = [%s]" %(key, ValDict[key])
