import glob, os
import sys
from gdcmPython import *

# Test each file of the Data directory

AllFiles = [
	"CR-MONO1-10-chest.dcm",
	"CT-MONO2-16-brain.dcm",
	"CT-MONO2-16-ort.dcm",
	"CT-MONO2-16-ankle.dcm",
	"CT-MONO2-8-abdo.dcm",
	"CT-MONO2-12-lomb-an2.acr2",
	"CT-MONO2-16-chest.dcm",
	"MR-MONO2-12-angio-an1.acr1",
	"MR-MONO2-12-an2.acr2",
	"MR-MONO2-16-head.dcm",
	"MR-MONO2-12-shoulder.dcm",
	"OT-PAL-8-face.dcm",
	"OT-MONO2-8-a7.dcm",
	"US-RGB-8-esopecho.dcm",
	"US-RGB-8-epicard.dcm",
	"MR-MONO2-8-16x-heart.dcm",
	"NM-MONO2-16-13x-heart.dcm",
	"US-MONO2-8-8x-execho.dcm",
	"US-PAL-8-10x-echo.dcm",
	"XA-MONO2-8-12x-catheter.dcm",
	]

if __name__ == '__main__':
   # AllFiles = glob.glob(os.path.join(GDCM_TEST_DATA_PATH,"*.dcm"))
   for file in AllFiles:
      fileName = os.path.join(GDCM_TEST_DATA_PATH, file)
      print "############## file :", fileName
      toRead = gdcmHeader(fileName)
      ValDict = toRead.GetPubElVal()
      for key in ValDict.keys():
          print "   [%s] = [%s]" %(key, ValDict[key])
