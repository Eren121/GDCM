import unittest
import os
from gdcmPython import *

class gdcmTestCase(unittest.TestCase):
   # The files whose name starts with a modality (e.g. CR-MONO1-10-chest.dcm)
   # come from Sebastien Barre's Dicom2 highly recommendable site
   # http://www.barre.nom.fr/medical/samples/index.html
   BarreFiles = [
      ####################################
      # CR modality examples:
      ####################################
      ["CR-MONO1-10-chest.dcm",
         [ ["Modality", "CR"],
           #"Transfer Syntax UID" is absent.
           ["Rows", "440"],
           ["Columns", "440"],
           ["Bits Stored", "10"],
           ["Bits Allocated", "16"],
           ["High Bit", "9"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "FUJI PHOTO FILM CO. LTD."],
           ["Manufacturer Model Name", "9000"],
           ["Pixel Data", "gdcm::NotLoaded. Address:776 Length:387200"] ] ],
      ####################################
      # CT modality examples:
      ####################################
      ["CT-MONO2-16-brain.dcm",
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"],  # Explicit VR, LE
           ["Modality", "CT"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "1"],
           ["Manufacturer", "Picker International, Inc."],
           ["Manufacturer Model Name", "PQ5000"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1680 Length:524288"] ] ],
      ["CT-MONO2-16-ort.dcm",
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2"],  # Implicit VR, LE
           ["Modality", "CT"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "1"],
           ["Manufacturer", "GE MEDICAL SYSTEMS"],
           ["Manufacturer Model Name", "HiSpeed CT/i"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1674 Length:524288"] ] ],
      ["CT-MONO2-16-ankle.dcm",
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2"],  # Implicit, little
           ["Modality", "CT"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "1"],
           ["Manufacturer", "GE MEDICAL SYSTEMS"],
           ["Manufacturer Model Name", "GENESIS_ZEUS"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1148 Length:524288"] ] ],
      ["CT-MONO2-8-abdo.dcm",
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2"],  # Implicit, little
           ["Modality", "CT"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "FUJI"],
           ["Pixel Data", "gdcm::NotLoaded. Address:796 Length:262144"] ] ],
      ["CT-MONO2-12-lomb-an2.acr2",
         [ ["Modality", "CT"],
           #"Transfer Syntax UID" and "Photometric Interpretation" are absent.
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "SIEMENS"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1230 Length:524288"] ] ],
      ["CT-MONO2-16-chest.dcm",
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.4.70"], # jpeg...
           ["Modality", "CT"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "400"],
           ["Columns", "512"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "1"],
           ["Manufacturer", "GE MEDICAL SYSTEMS"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1638 Length:143498"] ] ],
      ####################################
      ### MR modality examples:
      ####################################
      ["MR-MONO2-12-angio-an1.acr1",
         [ ["Recognition Code", "ACR-NEMA 1.0"],
           ["Modality", "MR"],
           #"Transfer Syntax UID" and "Photometric Interpretation" are absent.
           ["Rows", "256"],
           ["Columns", "256"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "12"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips"],
           # Note: 256*256*12/8 = 98304
           ["Pixel Data", "gdcm::NotLoaded. Address:650 Length:98304"] ] ],
      ["MR-MONO2-12-an2.acr2",
         [ ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "256"],
           ["Columns", "256"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "SIEMENS"],
           ["Manufacturer Model Name", "MAGNETOM VISION"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1866 Length:131072"] ] ],
      ["MR-MONO2-16-head.dcm",
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2"], # Imp VR, LE
           ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "256"],
           ["Columns", "256"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "1"],
           ["Manufacturer", "GE MEDICAL SYSTEMS"],
           ["Manufacturer Model Name", "GENESIS_SIGNA"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1804 Length:131072"] ] ],
      # MR-MONO2-12-shoulder example is present in the Jpeg section.
      # MR-MONO2-16-knee is not present in the test suite since it is too
      #    closely related to MR-MONO2-16-head.dcm to be of interest.
      ####################################
      # OT modality examples:
      ####################################
      # OT-MONO2-8-hip is not present in the test suite since it is too
      #    closely related to OT-MONO2-8-a7 to be of interest.
      ["OT-PAL-8-face.dcm",
         # Interest: has a color palette
         [#"Transfer Syntax UID", and "Manufacturer" are absent.
           ["Modality", "OT"],
           ["Photometric Interpretation", "PALETTE COLOR"],
           ["Rows", "480"],
           ["Columns", "640"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1654 Length:307200"] ] ],
      # OT-MONO2-8-colon is not present in the test suite since it is too
      #    closely related to OT-MONO2-8-a7 to be of interest.
      ["OT-MONO2-8-a7.dcm",
         [#"Transfer Syntax UID" is absent.
           ["Modality", "OT"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips Medical Systems"],
           ["Pixel Data", "gdcm::NotLoaded. Address:438 Length:262144"] ] ],
      ####################################
      # US (Ultra Sound) modality examples:
      ####################################
      ["US-RGB-8-esopecho.dcm",
         # Interest: RGB image
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"],  # Exp VR, LE
           ["Modality", "US"],
           ["Photometric Interpretation", "RGB"],
           ["Rows", "120"],
           ["Columns", "256"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Acme Products"],
         # FIXME: 92160 / (120*256) = 3 bytes per pixel NOT 1. Maybe
         # it has something to do with [Samples Per Pixel] = [3] ???
           ["Pixel Data", "gdcm::NotLoaded. Address:904 Length:92160"] ] ],
      ["US-RGB-8-epicard.dcm",
         # Interest: Big endian transfert syntax
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.2"],  # Big endian
           ["Modality", "US"],
           ["Photometric Interpretation", "RGB"],
           ["Rows", "480"],
           ["Columns", "640"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "G.E. Medical Systems"],
           ["Manufacturer Model Name", "LOGIQ 700"],
         # FIXME: 921600/(480*640) = 3 bytes per pixel NOT 1. Maybe
         # it has something to do with [Samples Per Pixel] = [3] ???
           ["Implementation Version Name", "OFFIS-DCMTK-311"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1012 Length:921600"] ] ],
   ]

   MultiFrameFiles = [
   # The number of images is contained in the "Number of Frames" element
      ["MR-MONO2-8-16x-heart.dcm",
         # Interest: multi-frame
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"], # ExpVR, LE
           ["Number of Frames", "16"],
           ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "256"],
           ["Columns", "256"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips"],
           # Note: 256*256*16=1048576 (16 is the number of Frames) 
           ["Pixel Data", "gdcm::NotLoaded. Address:920 Length:1048576"] ] ],
      ["NM-MONO2-16-13x-heart.dcm",
         # Interest: NM modality, multi-frame
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"], # ExpVR, LE
           ["Number of Frames", "13"],
           ["Modality", "NM"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "64"],
           ["Columns", "64"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "ACME Products"],
           # Note: 64*64*13*2=106496
           ["Pixel Data", "gdcm::NotLoaded. Address:1234 Length:106496"] ] ],
      ["US-MONO2-8-8x-execho.dcm",
         # Interest: multi-frame
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"], # ExpVR, LE
           ["Number of Frames", "8"],
           ["Modality", "US"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "120"],
           ["Columns", "128"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Acme Products"],
           ["Pixel Data", "gdcm::NotLoaded. Address:976 Length:122880"] ] ],
      ["US-PAL-8-10x-echo.dcm",
         # Interest: RLE (Run Length Encoded) compression, multiframe
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.5"], # RLE
           ["Number of Frames", "10"],
           ["Modality", "US"],
           ["Photometric Interpretation", "PALETTE COLOR"],
           ["Rows", "430"],
           ["Columns", "600"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "ACME Products"],
           ["Pixel Data", "gdcm::NotLoaded. Address:2428 Length:481182"] ] ],
      ["XA-MONO2-8-12x-catheter.dcm",
         # Interest: XA modality, multi-frame
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.4.70"], # jpeg
           ["Number of Frames", "12"],  # 12 images
           ["Modality", "XA"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Acme Products"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1006 Length:920072"] ] ],
   ]

   GdcmFiles = [
      ["gdcm-MR-SIEMENS-16.acr1",
         # Interest: good old Acr-Nema Version 1, see also Oddities below
         [ ["Recognition Code", "ACR-NEMA 1.0"],
           ["Modality", "MR"],
           ["Rows", "256"],
           ["Columns", "256"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "SIEMENS"],
           ["Manufacturer Model Name", "GBS III"],
         # FIXME: this image looks padded at the end. The length of the file
         # is 140288. Notice that, 256*256*2 + 1024 = 131072 + 1024 = 132096
           ["Pixel Data", "gdcm::NotLoaded. Address:8192 Length:132096"],
           # Oddities: "Study ID" and "Series Number" are empty
           ["Study ID", ""],
           ["Series Number", ""] ] ],
           # Oddities: "Study" and "Serie Instance UID" are not present
      ["gdcm-MR-SIEMENS-16.acr2",
         # Interest: Acr-Nema Version 2
         [ ["Recognition Code", "ACR-NEMA 2.0"],
           ["Modality", "MR"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "SIEMENS"],
           ["Manufacturer Model Name", "MAGNETOM VISION"],
           ["Study Instance UID",
            "1.3.12.2.1107.5.2.4.7630.20000918174641000"],
           ["Study ID", "1"],
           ["Series Instance UID",
            "1.3.12.2.1107.5.2.4.7630.20000918175714000007"],
           ["Series Number", "7"],
           ["Pixel Data", "gdcm::NotLoaded. Address:6052 Length:524288"] ] ],
      ["gdcm-US-ALOKA-16.dcm",
         # Interest: - possesses 3 LUTS: a Green (checked), a Red and BLue.
         #           - announced as implicit VR, but really explicit VR.
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2"],  # Implicit VR, LE
           ["Modality", "US"],
           ["Photometric Interpretation", "PALETTE COLOR"],
           ["Segmented Green Palette Color LUT Data",
            "gdcm::NotLoaded. Address:89576 Length:113784"],
           ["Rows", "480"],
           ["Columns", "640"],
           ["Bits Stored", "16"],
           ["Bits Allocated", "16"],
           ["High Bit", "15"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "ALOKA CO., LTD."],
           ["Manufacturer Model Name", "SSD-4000"],
           ["Pixel Data", "gdcm::NotLoaded. Address:258740 Length:614400"] ] ],
      ["gdcm-MR-PHILIPS-16.dcm",
         # Interest: - possesses a sequence
         #           - dicom file, with a recognition code of ACR-NEMA1
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2"],  # Implicit VR, LE
           ["Recognition Code", "ACR-NEMA 1.0"],
           ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "256"],
           ["Columns", "256"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "16"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips Medical Systems"],
           ["Manufacturer Model Name", "Gyroscan Intera"],
           ["Sequence Variant", "OTHER"],
           ["Pixel Data", "gdcm::NotLoaded. Address:6584 Length:131072"] ] ],
      ["gdcm-MR-PHILIPS-16-Multi-Seq.dcm",
         # Interest: - possesses many sequences in group 0x0029
         #           - Big sequence 28808 bytes at (0x0029, 0x263d)
         #           - dicom file, with a recognition code of ACR-NEMA1
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"],
           ["Recognition Code", "ACR-NEMA 1.0"],
           ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "128"],
           ["Columns", "128"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips Medical Systems"],
           ["Manufacturer Model Name", "Gyroscan Intera"],
           ["Sequence Variant", "OTHER"],
           ["Pixel Data", "gdcm::NotLoaded. Address:35846 Length:32768"] ] ],
      ["gdcm-MR-PHILIPS-16-NonRectPix.dcm",
         # Interest: - pixels are non rectangular
         #           - private elements are in explicit VR (which is normal)
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"],  # Explicit VR, LE
           ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "64"],
           ["Columns", "160"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips Medical Systems"],
           ["Manufacturer Model Name", "Gyroscan Intera"],
           ["Pixel Spacing", "0.487416\\0.194966"],
           ["Pixel Data", "gdcm::NotLoaded. Address:5010 Length:20480"] ] ],
      ["gdcm-CR-DCMTK-16-NonSamplePerPix.dcm",
         # Interest: - Misses the "Samples Per Pixel" element which prevents
         #             e-film from reading it.
         #           - Misses the Manufacturer related info
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.1"],  # Explicit VR, LE
           ["Modality", "CR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "750"],
           ["Columns", "750"],
           ["Bits Stored", "8"],
           ["Bits Allocated", "8"],
           ["High Bit", "7"],
           ["Pixel Representation", "0"],
           ["Implementation Version Name", "OFFIS_DCMTK_341"],
           ["Pixel Data", "gdcm::NotLoaded. Address:740 Length:562500"] ] ],
   ]

   GdcmJpegFiles = [
      ["gdcm-JPEG-Extended.dcm",
         # Interest: Jpeg compression [Extended (2,4)]
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.4.51"],
           ["Modality", "CT"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "SIEMENS"],
           ["Manufacturer Model Name", "Volume Zoom"],
           ["Pixel Data", "gdcm::NotLoaded. Address:2946 Length:192218"] ] ],
      ["MR-MONO2-12-shoulder.dcm",
         # Interest: Jpeg compression [Lossless, non-hierar. (14)]
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.4.57"],
           ["Modality", "MR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "1024"],
           ["Columns", "1024"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips Medical Systems"],
           ["Manufacturer Model Name", "Gyroscan NT"],
           ["Pixel Data", "gdcm::NotLoaded. Address:1580 Length:718948"] ] ],
      ["gdcm-JPEG-LossLess3a.dcm",
         # Interest: - Jpeg compression [Lossless, hierar., first-order
         #             pred. 14, Select. Val. 1]
         #           - contains a sequence [circa (0008,2112)]
         #           - contains additional data after the pixels ???
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.4.70"],
           ["Modality", "CT"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "512"],
           ["Columns", "512"],
           ["Bits Stored", "12"],
           ["Bits Allocated", "16"],
           ["High Bit", "11"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "SIEMENS"],
           ["Manufacturer Model Name", "Volume Zoom"] ] ],
         # FIXME: the pixel data looks like corrupted. As stated by "od -A d -c"
         #   0002528     à 177 020 \0  O  B \0 \0   ÿ  ÿ   ÿ   ÿ  þ  ÿ \0  à
         #   0002544    \0  \0  \0 \0  þ  ÿ \0  à 202  ? 003  \0  ÿ  Ø  ÿ  Ã
         # which we interpret as follows:
         #   - 2528: (à 177, 020 \0) = (0x7fe0, 0x0010) i.e. Pixel Data tag,
         #   - 2532: OB i.e. encapsulated encoded Pixel Data,
         #   - 2534: Two bytes reserved,
         #   - 2536: ÿ ÿ ÿ ÿ = 0xffffffff i.e. data element length is undefined.
         #   - 2540: (þ ÿ, \0 à) = (0xfffe, 0xe000) i.e. basic offset table
         #           item tag
         #   - 2544: \0 \0 \0 \0 i.e. offset table as length zero, i.e. no
         #           item value for offset table.
         #   - 2548: (þ ÿ, \0 à) = (0xfffe, 0xe000) i.e. item tag of first
         #           fragment of (encoded) pixel data.
         #   - 2552: 202 ? 003 \0 = 212866 bytes i.e. length of first fragment
         #           of (encoded) pixel data.
         # and so the next item tag should be found at the 2556+212866 th
         # octet i.e. at address 215422. But, as stated by od, we find:
         #   0215408   E   u Ö 026   Î   É 7 ¬ Ã ¸ ó ¿ ÿ Ù \f °
         #   0215424   ¶ 016 P   Ñ 002 016
         # and (\f °, ¶ 016) is NOT an OB item TAG which is required to be
         # present (at least there should be a sequence delimiter), refer to
         # PS 3.5-2001 page 50. 
      ["gdcm-JPEG-LossLess3b.dcm",
         # Interest: - Jpeg compression [Lossless, hierar., first-order
         #             pred. 14, Select. Val. 1]
         #           - encoding is sligthly different from LossLess3a.dcm ???
         [ ["Transfer Syntax UID", "1.2.840.10008.1.2.4.70"],
           ["Modality", "CR"],
           ["Photometric Interpretation", "MONOCHROME2"],
           ["Rows", "2076"],
           ["Columns", "1876"],
           ["Bits Stored", "15"],
           ["Bits Allocated", "16"],
           ["High Bit", "14"],
           ["Pixel Representation", "0"],
           ["Manufacturer", "Philips Medical Systems"],
           ["Manufacturer Model Name", "Cassette Holder Type 9840 500 35201"],
           ["Pixel Data", "gdcm::NotLoaded. Address:3144 Length:4795668"] ] ],
   ]

   def _BaseTest(self, FileSet):
      for entry in FileSet:
         fileName = os.path.join(GDCM_TEST_DATA_PATH, entry[0])
         toRead = gdcmHeader(fileName)
         valDict = toRead.GetPubElVal()
         for subEntry in entry[1]:
            element = subEntry[0]
            value   = subEntry[1]
            self.assertEqual(valDict[element], value, 
                             ("Wrong %s for file %s (got %s, shoud be %s)"
                               % (element,fileName, valDict[element], value)) )

   def testBarre(self):
      gdcmTestCase._BaseTest(self, gdcmTestCase.BarreFiles)

   def testMultiFram(self):
      gdcmTestCase._BaseTest(self, gdcmTestCase.MultiFrameFiles)

   def testFiles(self):
      gdcmTestCase._BaseTest(self, gdcmTestCase.GdcmFiles)

   def testJpeg(self):
      gdcmTestCase._BaseTest(self, gdcmTestCase.GdcmJpegFiles)

   def testWrite(self):
      import md5
      SourceFileName = os.path.join(GDCM_TEST_DATA_PATH,
                                    'gdcm-MR-PHILIPS-16-Multi-Seq.dcm')
      Source = gdcmFile(SourceFileName);
      Source.GetImageData()
      TargetFileName = "junk"
      Target = Source.WriteDcmImplVR(TargetFileName)
      Sign = '5af8739c15dd579dea223eb3930cacda'
      ComputeSign = md5.new(open(TargetFileName).read()).hexdigest()
      #print ComputeSign
      self.assertEqual(ComputeSign, Sign,
          ("Wrong signature for file %s (got %s, shoud be %s)"
           % (SourceFileName, ComputeSign, Sign)) )

if __name__ == '__main__':
   unittest.main()

