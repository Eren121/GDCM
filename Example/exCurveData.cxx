/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: exCurveData.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/16 17:07:05 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmCommon.h"
#include "gdcmDebug.h"
#include "gdcmDocEntry.h"
#include "gdcmBinEntry.h"

static const char* TypeOfDataArrays[13][2] = {
    { "TAC" , "time activity curve" },
    { "PROF" , "image profile" },
    { "HIST" , "histogram" },
    { "ROI" , "polygraphic region of interest" },
    { "TABL" , "table of values" },
    { "FILT" , "filter kernel" },
    { "POLY" , "poly line" },
    { "ECG" , "ecg data" },
    { "PRESSURE" , "pressure data" },
    { "FLOW" , "flow data" },
    { "PHYSIO" , "physio data" },
    { "RESP" , "Respiration trace" },
    { NULL, NULL }
};

// Part 3, C.10.2.1.1 Type of data
// Convert from acronym to full description
const char *ConvertTypeOfData(std::string const &type)
{
  const char **p = *TypeOfDataArrays;
  while(*p != NULL)
    {
    if( p[0] == type ) // std::string== operator
      {
      // ok we found it:
      return p[1];
      }
    p+=2;
    }
  return NULL;
}

// Helper function
template<class DataValueRepresentation>
inline size_t PrintCurveData(DataValueRepresentation* data, unsigned short numPts)
{
   for(unsigned int i=0; i<numPts;++i)
     {
     std::cout << "Pt(" << i <<  ") = " << data[i] << std::endl;
     }

   // ok this is ugly but I need the size outside of the function
   return sizeof(DataValueRepresentation);
}
 
/*
 // Example (sorry, we've got no more than this one ...)
 * V 5004|0000 [UL]                  [Group Length] [1998] x(7ce)
 * V 5004|0005 [US]              [Curve Dimensions] [1] x(1)
 * V 5004|0010 [US]              [Number of Points] [969] x(3c9)
 * V 5004|0020 [CS]                  [Type of Data] [PHYSIO]
 * V 5004|0022 [LO]             [Curve Description] []
 * V 5004|0103 [US]     [Data Value Representation] [0] x(0)
 * B 5004|3000 [OW]                    [Curve Data] [gdcm::Binary data loaded;length = 1938]
 */
 
int main(int argc, char *argv[])
{  
   gdcm::File *f;
 
   std::cout << "------------------------------------------------" << std::endl;
   std::cout << "Gets the 'Curve Data' from a full gdcm-readable DICOM " << std::endl;
   std::cout << "(Note :  we just have ONE image : "
             << "GE_DLX-8-MONO2-Multiframe.dcm"
             << std::endl;
   std::cout << "------------------------------------------------" << std::endl;

   std::string fileName;
   if( argc > 1 )
      fileName = argv[1];
   else
      fileName = "GE_DLX-8-MONO2-Multiframe.dcm";

   std::cout << fileName << std::endl;
// ============================================================
//   Read the input image.
// ============================================================

   f = new gdcm::File( );

   f->SetLoadMode(gdcm::LD_NOSEQ | gdcm::LD_NOSHADOW);
   f->SetFileName( fileName );
   bool res = f->Load();  

   if( gdcm::Debug::GetDebugFlag() )
   {
      std::cout << "---------------------------------------------" << std::endl;
      f->Print();
      std::cout << "---------------------------------------------" << std::endl;
   }
   if (!res) {
       std::cout << "Sorry, " << fileName <<"  not a gdcm-readable "
           << "DICOM / ACR File"
           <<std::endl;
      delete f;
      return 1;
   }
   std::cout << " ... is readable " << std::endl;

// ============================================================
//   Check whether image contains Overlays ACR-NEMA style.
// ============================================================

   //* B 5004|3000 [OW]                    [Curve Data] [gdcm::Binary data loaded;length = 1938]
   std::string curve_data_str = f->GetEntryValue(0x5004, 0x3000);
   if (curve_data_str == gdcm::GDCM_UNFOUND)
   {
      std::cout << " Image doesn't contain any Curve Data" << std::endl;
      delete f;
      return 1;
   }
   std::cout << " File is read! " << std::endl;


// ============================================================
//   Load the Curve Data in memory.
// ============================================================
  std::istringstream convert;
 //* V 5004|0005 [US]              [Curve Dimensions] [1] x(1)
   std::string curve_dim_str = f->GetEntryValue(0x5004,0x0005);
   unsigned short curve_dim;
   convert.str(curve_dim_str);
   convert >> curve_dim;
   std::cout << "Curve Dimensions: " << curve_dim << std::endl;
 //* V 5004|0010 [US]              [Number of Points] [969] x(3c9)
   std::string num_points_str = f->GetEntryValue(0x5004,0x0010);
   unsigned short num_points;
   convert.clear(); //important
   convert.str(num_points_str);
   convert >> num_points;
   std::cout << "Number of Points: " << num_points << std::endl;
 //* V 5004|0020 [CS]                  [Type of Data] [PHYSIO]
   std::string data_type = f->GetEntryValue(0x5004,0x0020);
   std::cout << "Type of Data: " << data_type << std::endl;
   std::cout << " this is thus a : " << ConvertTypeOfData(data_type) << std::endl;
 //* V 5004|0022 [LO]             [Curve Description] []
   std::string curve_desc = f->GetEntryValue(0x5004,0x0022);
   std::cout << "Curve Description: " << curve_desc << std::endl;
 //* V 5004|0103 [US]     [Data Value Representation] [0] x(0)
   std::string data_rep_str = f->GetEntryValue(0x5004,0x0103);
   unsigned short data_rep;
   convert.clear(); //important
   convert.str(data_rep_str);
   convert >> data_rep;


   gdcm::DocEntry *pCurveDataDoc = f->GetDocEntry(0x5004, 0x3000);
   gdcm::BinEntry *pCurveData = dynamic_cast<gdcm::BinEntry*>(pCurveDataDoc);
   uint8_t *curve_data = pCurveData->GetBinArea();
   
   // From Part3, C.10.2.1.2 Data value representation (p668)
   size_t sz;
   switch( data_rep)
     {
   case 0:
     sz = PrintCurveData((unsigned short*)(curve_data), num_points);
     break;
   case 1:
     sz = PrintCurveData((signed short*)(curve_data), num_points);
     break;
   case 2:
     sz = PrintCurveData((float*)(curve_data), num_points);
     break;
   case 3:
     sz = PrintCurveData((double*)(curve_data), num_points);
     break;
   case 4:
     sz = PrintCurveData((signed long*)(curve_data), num_points);
     break;
   default:
     std::cerr << "Error don't know the type: " << data_rep_str << std::endl;
     delete f;
     return 1;
     }
   // Just to make sure that values read are consistant and we won't read out of bound data:
   assert( sz*num_points == pCurveData->GetLength());

   // Write out the data as a file:
   //std::ofstream o("/tmp/curve_data.raw");
   //o.write((char*)curve_data, num_points*sz);
   //o.close();

   delete f;
   return 0;
}


