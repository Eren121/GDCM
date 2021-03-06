/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: PcpdenseToDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2011/09/12 23:27:41 $
  Version:   $Revision: 1.11 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include <fstream>
#include <iostream>
//#include <values.h>

#if defined(__BORLANDC__)
#include <ctype.h>
#endif

#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"
#include "gdcmDirList.h"
#include "gdcmUtil.h"
#include "gdcmArgMgr.h"

/**
  * \brief   
  *  Converts the "pcpdense" ".txt" (2008 version)  files into 16 bits Dicom Files,
  * Hope they don't change soon!
  */  

void MakeDicomImage(unsigned short int *tabVal, int X, int Y, std::string dcmImageName, const char * patientName, int nbFrames,
                    std::string studyUID, std::string serieUID, std::string SerieDescr, int imgNum, bool m );

void LoadImage(std::ifstream &from,  unsigned short int *, int multFact);

void LoadImageX2(std::ifstream &from, unsigned short int *, int multFact);      
void RotateImage(unsigned short int *image, unsigned short int *image2, int NX, int NY);
void FlipImage  (unsigned short int *image, unsigned short int *image2, int NX, int NY);

void WholeBazar (unsigned short int *image, int NX, int NY, int numberOfSlices, std::string strStudyUID, std::string serieDescr, const char* patientName, bool multiframe, bool X2, int multFact, const char *rootfilename);
bool verbose;

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n pcpdenseToDicom :\n                                                   ",
   "        Converts the '.txt' files into 16 bits Dicom-like Files,          ",
   " usage:                                                                   ",
   " pcpdenseToDicom rootfilename=...                                         ",
   "                 (e.g.. :   meas_MID380_DENSE_stacked_slices_aif_FID81637)",
   "                 numberOfSlices =  (default : 3)                          ",
   "                 X2 : multiply x 2 image size                             ",
   "                 multFact = (default : 1000) multiply pixel value by ...  ",
   "                 m :create multiframe files instead of image stacks       ", 
   "                 [patientname = Patient's name]                           ",
   "                 [verbose] [debug]                                        ",
   "                                                                          ",
   " verbose  : user wants to run the program in 'verbose mode'               ",
   " debug    : *developer*  wants to run the program in 'debug mode'         ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------
      
   GDCM_NAME_SPACE::ArgMgr *am = new GDCM_NAME_SPACE::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }
   // Seems that ArgMgrWantString doesn't work on MacOS   
   if(!am->ArgMgrDefined("rootfilename"))
   {
      std::cout << "'rootfilename' is mandatory" << std::endl;
      exit(0);   
   }
 
   const char *rootfilename = am->ArgMgrWantString("rootfilename",usage);
   int numberOfSlices       = am->ArgMgrGetInt("numberOfSlices",3);
   int multFact             = am->ArgMgrGetInt("multFact",1000); 
   const char *patientName  = am->ArgMgrGetString("patientname", "Patient^Name");
         
   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();

   verbose         = ( 0 != am->ArgMgrDefined("verbose") );    
   bool X2         = ( 0 != am->ArgMgrDefined("X2") );
   bool multiframe = ( 0 != am->ArgMgrDefined("m") );

   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }
   delete am;  // we don't need Argument Manager any longer

   // ----- Begin Processing -----
   
   std::ostringstream Ecc;
   std::ostringstream perf;
   std::ostringstream WashoutTc;

   std::string strSerieUID; 
   std::string strStudyUID =  GDCM_NAME_SPACE::Util::CreateUniqueUID();
   std::string /*dcmImageName,*/ textFileName, patientname, serieDescr;
   std::string deb(rootfilename);
   
   unsigned short int *image;
//   unsigned short int *image2;
  
   int NX, NY;
     
  // Get some info
  // -------------

   {
        Ecc.str(rootfilename); 
        Ecc   << Ecc.str() << "_s0" << "_Ecc.txt";

        std::ifstream fromEcc( Ecc.str().c_str() );             
        if ( !fromEcc )
        {
           std::cout << "Can't open file [" << Ecc.str() << "]" << std::endl;
           exit(0);
        }
        std::string str1;

         fromEcc >> str1;
         fromEcc >> str1;

         fromEcc >> NX;
         fromEcc >> NY;
         std::cout << "NX, NY : " << NX << ", " << NY << std::endl; 
   }

   
serieDescr = "Ecc";      
WholeBazar(image,  NX,  NY,  numberOfSlices, strStudyUID, serieDescr, patientName,  multiframe, X2, multFact, rootfilename);

serieDescr = "perf";
WholeBazar(image,  NX,  NY,  numberOfSlices, strStudyUID, serieDescr, patientName,  multiframe, X2, multFact, rootfilename);

serieDescr = "WashoutTc";
WholeBazar(image,  NX,  NY,  numberOfSlices, strStudyUID, serieDescr, patientName,  multiframe, X2, multFact, rootfilename);

  // delete []image;
   return 1;            
}

// =====================================================================================================================

void LoadImage(std::ifstream &from,  unsigned short int *image, int multFact)
{
// in any file ".txt" :

/*
XY Dimensions           47          50
     0.000000     0.000000     0.000000     0.000000     0.000000     0.000000
     ...
*/  

   if (!from)
      return;
   
   std::string str1;

   from >> str1;
   from >> str1;
    
   int NX, NY;
   from >> NX;
   from >> NY;
   std::cout << "NX, NY : " << NX << ", " << NY << std::endl; 

   float pixelValue;
     
    int i, j;
    for( i=0;i<NY;i++) {
        for(j=0;j<NX;j++) {
           from >> pixelValue;
           image[i*NX+j] = (unsigned short int)(pixelValue * multFact); // Why do we multiply by 1000? // JPR
        }
     }
}


// =====================================================================================================================

void LoadImageX2(std::ifstream &from,  unsigned short int *image, int multFact )
{
// in any file ".txt" :

/*
XY Dimensions           47          50
     0.000000     0.000000     0.000000     0.000000     0.000000     0.000000
     ...
*/  

   if (!from)
      return;

   std::string str1;

    from >> str1;
    from >> str1;
    
    int NX, NY;
    from >> NX;
    from >> NY;
    std::cout << "NX, NY : " << NX << ", " << NY << std::endl; 

     int k;
     for( k=0;k<NX*NY*4;k++)
          image[k] = 0;

    float pixelValue;
     
     int i, j;
 /*
     for( i=0;i<NY;i++) {
           for(j=0;j<NX;j++) {
              from >> pixelValue;
              pixelValue*=1000.;  // Why do we multiply by 1000? // JPR
              image[i*4*NX + j*2] = image[i*4*NX + j*2+1] =  image[(i*4+2)*NX + j*2] =  image[(i*4+2)*NX + j*2+1] = (unsigned short int)(pixelValue);  
           }
     }
 */
 
     int lgrLigneNvlleImage = NX+NX;
     int debLigneNvlleImage = 0;
     for( i=0;i<NY;i++) {
           for(j=0;j<NX;j++) {
              from >> pixelValue;
              pixelValue*=multFact;  // Why do we multiply by 1000? // JPR
              image[debLigneNvlleImage + j+j] = 
              image[debLigneNvlleImage + j+j +1] = 
              image[debLigneNvlleImage + lgrLigneNvlleImage +j+j] = 
              image[debLigneNvlleImage + lgrLigneNvlleImage + j+j +1] = 
                 (unsigned short int)(pixelValue);
           }
           debLigneNvlleImage += 2*lgrLigneNvlleImage;  
     }
}
// =====================================================================================================================================
void RotateImage(unsigned short int *image, unsigned short int *image2, int NX, int NY)
{
     int k = 0;
     for( int i=0;i<NY;i++) {
           for(int j=0;j<NX;j++) {
              image2[NY*j + i] = image[k];
              k++;   
           }
     }
}


// =====================================================================================================================================
void FlipImage(unsigned short int *image, unsigned short int *image2, int NX, int NY)
{
     unsigned short int temp;
     for(int i=0;i<NY/2;i++) {
           for(int j=0;j<NX;j++) {
              temp = image[NX*i + j];
              image2[NX*i + j] = image[NX*(NY-i-1) + j];
              image2[NX*(NY-i-1) + j] = temp;
           }
     }
}

// =====================================================================================================================================

void WholeBazar(unsigned short int *image, int NX, int NY, int numberOfSlices, std::string strStudyUID, std::string serieDescr, const char *patientName, bool multiframe, bool X2, int multFact, const char *rootfilename)
{   
   unsigned short int *image2;
   std::string strSerieUID =  GDCM_NAME_SPACE::Util::CreateUniqueUID();
   std::string deb(rootfilename);
   std::ostringstream Ecc;
   std::ostringstream trueSerieDescr;   
   std::string dcmImageName;

   int mult;
   if (X2)
      mult=4;
   else
      mult=1;

   if (multiframe) {
      image  = new unsigned short int[NX*NY*mult*numberOfSlices];
      image2 = new unsigned short int[NX*NY*mult*numberOfSlices];
   } else {
      image  = new unsigned short int[NX*NY*mult];
      image2 = new unsigned short int[NX*NY*mult];
   }
   
   if (!multiframe) {
     for (int i=0; i<numberOfSlices; i++)
     {  
        Ecc.str(rootfilename); 
        //Ecc << Ecc.str() << "_s" << i << "_Ecc.txt";
        Ecc << Ecc.str() << "_s" << i << "_" << serieDescr << ".txt";

        trueSerieDescr.str("");

        trueSerieDescr  << serieDescr << "_" "s" << i;
        //std::cout << "-------------------------------------------------------" << trueSerieDescr.str() << std::endl;
    
        std::ifstream fromEcc( Ecc.str().c_str() );             
        if ( !fromEcc )
        {
           std::cout << "Can't open file [" << Ecc.str() << "]" << std::endl;
           exit(0);
        }
        std::cout << "Open file [" << Ecc.str() << "] : OK" << std::endl;
        dcmImageName = Ecc.str() + ".dcm";

        if (X2)
        {
           LoadImageX2(fromEcc, image, multFact);
           RotateImage(image, image2, NX*2, NY*2);
           FlipImage(image2,  image2, NY*2, NX*2);  
           MakeDicomImage(image, NY*2, NX*2, dcmImageName, patientName, 1, strStudyUID, strSerieUID, trueSerieDescr.str(), i, multiframe );
        }
        else
        {  
           LoadImage(fromEcc, image, multFact);
           RotateImage(image, image2, NX, NY);
           FlipImage(image2,  image2, NY, NX);   
           MakeDicomImage(image2, NY, NX,     dcmImageName, patientName, 1, strStudyUID, strSerieUID, trueSerieDescr.str(), i, multiframe );
        }         
        fromEcc.close();

     } // end : for (int i=0; i<numberOfSlices
   }
   
   if (multiframe) {  
     for (int i=0; i<numberOfSlices; i++)
     {  
        Ecc.str(rootfilename);
        //Ecc   << Ecc.str()    << "_s" << i << "_Ecc.txt";
        Ecc   << Ecc.str()    << "_s" << i << "_" << serieDescr << ".txt";
      
        std::ifstream fromEcc( Ecc.str().c_str() );             
        if ( !fromEcc )
        {
           std::cout << "Can't open file [" << Ecc.str() << "]" << std::endl;
           exit(0);
        }

        std::cout << "Open file [" << Ecc.str() << "] : OK" << std::endl;
        if (X2)
        {
          LoadImageX2(fromEcc ,&image[NX*NY*4*i] , multFact);
          RotateImage(&image[NX*NY*4*i], &image2[NX*NY*4*i], NX*2, NY*2);  
          FlipImage( &image2[NX*NY*4*i], &image2[NX*NY*4*i], NY*2, NX*2);  
        }
        else
        {
          LoadImage(fromEcc, &image[NX*NY*i], multFact );
          RotateImage(&image[NX*NY*i], &image2[NX*NY*i], NX, NY);  
          FlipImage( &image2[NX*NY*i], &image2[NX*NY*i], NY, NX);  
        }
        
        fromEcc.close();
     } // end : for (int i=0; i<numberOfSlices
     
 //    dcmImageName = deb + "_Ecc.dcm";
     dcmImageName = deb + "_" +  serieDescr + ".dcm";     
     if (X2)     
        MakeDicomImage(image2, NY*2, NX*2, dcmImageName, patientName, numberOfSlices, strStudyUID, strSerieUID, serieDescr, 0, multiframe );
     else
        MakeDicomImage(image2, NY, NX,     dcmImageName, patientName, numberOfSlices, strStudyUID, strSerieUID, serieDescr, 0, multiframe );   
   }  // end : if (multiframe)
}

//=====================================================================================================================================

void MakeDicomImage(unsigned short int *tabVal, int X, int Y, std::string dcmImageName, const char * patientName, int nbFrames, std::string studyUID, std::string serieUID, std::string SerieDescr, int imgNum, bool m)
{

//std::cout << "========================> in MakeDicomImage : dcmImageName = [" << dcmImageName << "] NX= " << X << " NY= " << Y << std::endl;
 // GDCM_NAME_SPACE::Debug::DebugOn();
  
   std::ostringstream str;

   GDCM_NAME_SPACE::File *file;
   file = GDCM_NAME_SPACE::File::New();       
      
  // Set the image size
   str.str(""); 
   str << X;
   file->InsertEntryString(str.str(),0x0028,0x0011,"US"); // Columns
   str.str("");
   str << Y;
   file->InsertEntryString(str.str(),0x0028,0x0010,"US"); // Rows

  // Set the pixel type
  //      16; //8, 16, 32
   file->InsertEntryString("16",0x0028,0x0100,"US"); // Bits Allocated
   
   str.str("");
   str << 16; // may be 12 or 16 if componentSize =16
   file->InsertEntryString("16",0x0028,0x0101,"US"); // Bits Stored
   file->InsertEntryString("15",0x0028,0x0102,"US"); // High Bit

  // Set the pixel representation // 0/1 , 0=unsigned
   file->InsertEntryString("1",0x0028,0x0103, "US"); // Pixel Representation
   
  // Set the samples per pixel // 1:Grey level, 3:RGB
   file->InsertEntryString("1",0x0028,0x0002, "US"); // Samples per Pixel


   if (nbFrames != 1)
   {
      str.str("");
      str << nbFrames;
      file->InsertEntryString(str.str(),0x0028,0x0008,"IS"); // Number of Frames  
   }
  
   if (strlen(patientName) != 0)
      file->InsertEntryString(patientName,0x0010,0x0010, "PN"); // Patient's Name

   file->InsertEntryString(studyUID, 0x0020, 0x000d, "UI");
   file->InsertEntryString(serieUID, 0x0020, 0x000e, "UI");
 
   file->InsertEntryString(SerieDescr,0x0008,0x103e, "LO");  // Series Description 

// 0020 0037 DS 6 Image Orientation (Patient)   
   file->InsertEntryString("1.0\\0.0\\0.0\\0.0\\1.0\\0.0",0x0020,0x0037, "DS"); //[1\0\0\0\1\0] : Axial   (Tant pis!)
   
// 0020 0032 DS 3 Image Position (Patient)   
   char charImagePosition[256];
   sprintf(charImagePosition,"0.0\\0.0\\%f",(float)imgNum);
   file->InsertEntryString(charImagePosition,0x0020,0x0032, "DS");  //0020 0032 DS 3 Image Position (Patient)        

// 0020 0x1041 DS 1 Slice Location 
        sprintf(charImagePosition,"%f",float(imgNum));
        file->InsertEntryString(charImagePosition,0x0020,0x1041, "DS");   
/*
  // Set Rescale Intercept
        str.str("");
        str << div;  
        file->InsertEntryString(str.str(),0x0028,0x1052,"DS");

  // Set Rescale Slope
        str.str("");
        str << mini;  
        file->InsertEntryString(str.str(),0x0028,0x1053,"DS");
*/

   GDCM_NAME_SPACE::FileHelper *fileH;
   fileH = GDCM_NAME_SPACE::FileHelper::New(file);
   // cast is just to avoid warnings (*no* conversion is performed)
   //fileH->SetImageData((uint8_t *)img,int(maxX*maxY)*sizeof(uint16_t)); // troubles when maxX, mayY are *actually* float!
   
   fileH->SetImageData((uint8_t *)tabVal,X*Y*nbFrames*sizeof(uint16_t));
   fileH->SetWriteModeToRaw(); 
   fileH->SetWriteTypeToDcmExplVR();
        
   if( !fileH->Write(dcmImageName))
      std::cout << "Failed for [" << dcmImageName << "]\n"
                << "           File is unwrittable" << std::endl;

  // file->Print();
           
  // delete img;
   file->Delete();
   fileH->Delete(); 
   //std::cout << "========================> out of MakeDicomImage : " << std::endl; 
}


// =====================================================================================================================
