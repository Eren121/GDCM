from gdcmPython import *

print "##############################################################"
print "### Display all the possible tags of the current public"
print "### dictionary"
print "##############################################################"
PubList = GetPubDictTagNames()
for i in range(0, len(PubList)):
   print "   ", PubList[i]

print "##############################################################"
print "### Display all the possible tags of the current public"
print "### dictionary, but grouped by using the Fourth field"
print "##############################################################"
print " WARNING : the 'fourth' fiels IS NOT part of DICOM"
print " DO NOT use it"
PubDict = GetPubDictTagNamesByCategory()
for fourth in PubDict:
   print "   ############ Fourth group = ", fourth, " ##############"
   for key in PubDict[fourth]:
      print "   ", key
