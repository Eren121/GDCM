# Rename the dcm extension files of a given directory according
# to series and image number information extracted in each respective header.

from gdcmPython import *
import glob
import os
import sys

def PrintUsage():
   print "Usage: python ReorganiseFiles.py <SourceDirectory>"
   print "       <SourceDirectory> is the directory containing the source"
   print "                         images (defaulted to invocation directory."

def Warning(DirName):
   print "Warning: this script will rename files in the"
   print "        ", DirName, " directory."
   a = raw_input("Proceed ?[y/n]")
   if a != "y":
      sys.exit()

try:
   SourceDirectory = sys.argv[1]
except IndexError:
   SourceDirectory = os.getcwd()

if not os.path.isdir(SourceDirectory):
   PrintUsage
Warning(SourceDirectory)
SourceFiles=glob.glob(os.path.join(SourceDirectory, "*.dcm"))

for file in SourceFiles:
   header = gdcmHeader(file)
   info = header.GetPubEntry()
   try:
      ImNum = info["Image Number"]
      if len(ImNum) == 0 or ImNum == "gdcm::Unfound":
         raise KeyError
   except KeyError:
      print "Skipped file ", file, " (unfound Image Number tag)."
      continue
   try:
      SeNum = info["Series Number"]
      if len(SeNum) == 0 or SeNum == "gdcm::Unfound":
         raise KeyError
   except KeyError:
      print "Skipped file ", file, " (unfound Series Number tag)."
      continue
   DestFile = "image_%02d_%04d.dcm"
   DestFile %= (int(info["Image Number"]), int(info["Series Number"]))
   DestFile = os.path.join(SourceDirectory, DestFile)
   # necessary to close the file descriptor to enable further rename.
   del header
   if os.path.isfile(DestFile):
      print "Skipped file ", file, " (allready existing destination"
      print "     file ", DestFile
      continue
   os.rename(file, DestFile)
