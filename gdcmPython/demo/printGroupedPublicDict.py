from load import *
toRead = gdcm.gdcmHeader(FileName)
toRead.LoadElements()

print "##############################################################"
print "### Display all the possible tags of the current public"
print "### dictionary, but grouped by using the Fourth field"
print "##############################################################"
AllDictFields = toRead.GetPubTagNamesByCategory()
for fourth in AllDictFields.keys():
	print "   ############ Fourth group = ", fourth, " ##############"
	for entry in  AllDictFields[fourth]:
		print "   ", entry

