/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: Dense2007ToDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2007/07/10 11:00:50 $
  Version:   $Revision: 1.1 $
                                                                                
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

#include "gdcmFile.h"
#include "gdcmFileHelper.h"
#include "gdcmDebug.h"
#include "gdcmDirList.h"

#include "gdcmArgMgr.h"

/**
  * \brief   
  *  Converts the "Dense" ".txt" (2007 version)  files into 16 bits Dicom Files,
  * Hope they don't change soon!
  */  


void LoadPeakStrain(std::ifstream &from, std::string imageName);
void LoadStrain(std::ifstream &from, std::string imageName);
void MakeDicomImage(float *tabVal, float *X, float *Y, float *Z, int NP, std::string dcmImageName);


int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n Dense2007ToDicom :\n                                                  ",
   "        Converts the '.txt' files into 16 bits Dicom Files,               ",
   " usage:                                                                   ",
   " Dense2007ToDicom strain=...strain.txt  peak_strain=...peak_strain.txt    ",
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

   const char *strain      = am->ArgMgrWantString("strain",usage);
   const char *peak_strain = am->ArgMgrWantString("peak_strain",usage);
   
   if (am->ArgMgrDefined("debug"))
      GDCM_NAME_SPACE::Debug::DebugOn();
      
   int verbose  = am->ArgMgrDefined("verbose");      

   
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }
   delete am;  // we don't need Argument Manager any longer

   // ----- Begin Processing -----


      std::ifstream fromPeakStrain( peak_strain );             
      if ( !fromPeakStrain )
      {
         std::cout << "Can't open file" << peak_strain << std::endl;
         exit(0);
      }
      
      std::ifstream fromStrain( strain );      
      if ( !fromStrain )
      {
         std::cout << "Can't open file" << strain << std::endl;
          exit(0);
     }      
            
      std::cout << "Success in open file" << peak_strain << std::endl;
      LoadPeakStrain(fromPeakStrain, peak_strain);
      fromPeakStrain.close();  
     
      std::cout << "Success in open file" << strain << std::endl;
      LoadStrain(fromStrain, strain);
      fromStrain.close();      
               
}

// =====================================================================================================================

void LoadPeakStrain(std::ifstream &from, std::string textFileName)
{
// in sax_base_slice0_peak_strain.txt :

/*
Number of material points (NP) = 181
Origin of (readout, phase enc, slice sel) coordinates in 3D =  87.3243 3.19392
88.2381
Readout direction in 3D =  -0.162314 -0.0771294 -0.983720
Phase Enc. direction in 3D =  -0.540606 -0.827052 0.154046
Slice select direction in 3D =  0.825469 -0.556809 -0.0925458
The following are the (readout, phase enc, slice sel) coordinates (mm) of the grid points for which strains are calculated,
followed by their peak Ecc strain, an array of NP elements,
followed by their peak Err strain, an array of NP elements,
followed by their peak E11 strain, an array of NP elements,
followed by their Peak E22 strain, an array of NP elements,
      42.0000      10.0000     0.000000
      ...
    -0.154905   -0.0840482    -0.157350    -0.221403    -0.168118    -0.131331
    -0.153781    -0.148481    -0.166602    -0.232858    -0.222650    -0.213712
    ...
*/  

   if (!from)
      return;

   std::string str1;
   int NP;

   //Number of material points (NP) = 181   
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> NP;
    
    std::cout << "NP : " << NP << std::endl; 

   //Origin of (readout, phase enc, slice sel) coordinates in 3D =  87.3243 3.19392 88.2381
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     
     float readout,  phase_enc, slice_sel;
     from >> readout;
     from >> phase_enc;
     from >> slice_sel;
     std::cout << " readout " << readout << " phase_enc " << phase_enc << " slice_sel " << slice_sel << std::endl;
     
    // Readout direction in 3D =  -0.162314 -0.0771294 -0.983720
    
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    
    float readoutX, readoutY, readoutZ;
    from >> readoutX;
    from >> readoutY;       
    from >> readoutZ;
    std::cout << " readoutX " << readoutX <<  " readoutY " << readoutY <<  " readoutZ " << readoutZ << std::endl;
     
// Phase Enc. direction in 3D =  -0.540606 -0.827052 0.154046

     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     
    float phase_encX, phase_encY, phase_encZ;
    from >> phase_encX;
    from >> phase_encY;       
    from >> phase_encZ;
    std::cout << " phase_encX " << phase_encX <<  " phase_encY " << phase_encY <<  " phase_encZ " << phase_encZ << std::endl; 

// Slice select direction in 3D =  0.825469 -0.556809 -0.0925458
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     
    float slice_selX, slice_selY, slice_selZ;
    from >> slice_selX;
    from >> slice_selY;       
    from >> slice_selZ;
    std::cout << " slice_selX " << slice_selX <<  " slice_selY " << slice_selY <<  " slice_selZ " << slice_selZ << std::endl; 



// Skip 5 lines :
/*
The following are the (readout, phase enc, slice sel) coordinates (mm) of the grid points for which strains are calculated,
followed by their peak Ecc strain, an array of NP elements,
followed by their peak Err strain, an array of NP elements,
followed by their peak E11 strain, an array of NP elements,
followed by their Peak E22 strain, an array of NP elements,
*/

std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------stop skipping ---------------- " << std::endl;
  
   float *X = new float[NP];
   float *Y = new float[NP];   
   float *Z = new float[NP];
      
   char c;
   int i;   
   for (i=0; i<NP; i++) {   
      from >> X[i];
      for (;;) {
        if (!from.get(c))
          break;
        if (!isspace(c)) {
          from.putback(c);
          break;
        }
     }  
      from >> Y[i];    
      for (;;) {
        if (!from.get(c))
          break;
        if (!isspace(c)) {
          from.putback(c);
          break;
        }
     }        
      from >> Z[i];              
   }

   std::cout << "--------------- Ecc_strain ------------------" << std::endl;
   float *ecc_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> ecc_strain[i]; 
       std::cout <<  ecc_strain[i] <<  std::endl;
   }

   std::cout << "--------------- Err_strain ------------------" << std::endl;
   float *err_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> err_strain[i]; 
       std::cout <<  err_strain[i] <<  std::endl;
   }
   
   std::cout << "--------------- E11_strain ------------------" << std::endl;
   float *e11_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> e11_strain[i]; 
       std::cout <<  e11_strain[i] <<  std::endl;
   }  
   
   std::cout << "--------------- E22_strain ------------------" << std::endl;
   float *e22_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> e22_strain[i]; 
       std::cout <<  e22_strain[i] <<  std::endl;
   }    
 
   std::string dcmImageName;    

//followed by their peak Ecc strain, an array of NP elements,
   dcmImageName = textFileName + "_peak_Ecc_strain.dcm";
   MakeDicomImage(ecc_strain, X, Y, Z, NP, dcmImageName);

//followed by their peak Err strain, an array of NP elements,
   dcmImageName = textFileName + "_peak_Err_strain.dcm";
   MakeDicomImage(err_strain, X, Y, Z, NP, dcmImageName);
   
//followed by their peak E11 strain, an array of NP elements,
   dcmImageName = textFileName + "_peak_E11_strain.dcm";
   MakeDicomImage(e11_strain, X, Y, Z, NP, dcmImageName);
   
//followed by their Peak E22 strain, an array of NP elements,
   dcmImageName = textFileName + "_peak_E22_strain.dcm";
   MakeDicomImage(e22_strain, X, Y, Z, NP, dcmImageName);         
}

// =====================================================================================================================

void LoadStrain(std::ifstream &from, std::string textFileName)
{

// in sax_base_slice0_strain.txt :
/*
Number of cine frames = 18
Temporal resolution = 32.0000 ms
First frame starts at 48.0000 ms
Number of material points (NP) = 181
Origin of (readout, phase enc, slice sel) coordinates in 3D =  87.324341 3.193918 88.238113 
Readout direction in 3D =  -0.162314 -0.0771294 -0.983720
Phase Enc. direction in 3D =  -0.540606 -0.827052 0.154046
Slice select direction in 3D =  0.825469 -0.556809 -0.0925458
The following are the (readout, phase enc, slice sel) coordinates (mm) of the grid points for which strains are calculated,
followed by their Ecc strain, an array of dimensions(NP, number of cine frames),
followed by their Err strain, an array of dimensions(NP, number of cine frames),
followed by their E11 strain, an array of dimensions(NP, number of cine frames),
followed by their E22 strain, an array of dimensions(NP, number of cine frames),
Note that RV Err, E11 and E22 strains are not calculated due to the small thickness.
      42.0000      10.0000     0.000000
      44.0000      10.0000     0.000000
      ...
   -0.0622793   -0.0840482    -0.157350    -0.196722    -0.105844    -0.131331
    -0.153781  -0.00940573   -0.0542236    -0.100403   -0.0369671   -0.0696840      
*/ 

   if (!from)
      return;

   std::string str1;
   int NP;    // Number of Pints
   int NCF;   // Number of cine frames
   float TR;  // Temporal resolution
   float FFS; // First frame starts
   
   // Number of cine frames = 18
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> NCF;

   // Temporal resolution = 32.0000 ms
    from >> str1;
    from >> str1;
    from >> str1;
    from >> TR;
    from >> str1;

   // First frame starts at 48.0000 ms
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> FFS;
    from >> str1;        

   //Number of material points (NP) = 181   
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> NP;
    
    std::cout << "NP : " << NP << std::endl; 

   //Origin of (readout, phase enc, slice sel) coordinates in 3D = 87.324341 3.193918 88.238113 
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     
     float readout,  phase_enc, slice_sel;
     from >> readout;
     from >> phase_enc;
     from >> slice_sel;
     std::cout << " readout " << readout << " phase_enc " << phase_enc << " slice_sel " << slice_sel << std::endl;
     
    // Readout direction in 3D =  -0.162314 -0.0771294 -0.983720
    
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1;
    
    float readoutX, readoutY, readoutZ;
    from >> readoutX;
    from >> readoutY;       
    from >> readoutZ;
    std::cout << " readoutX " << readoutX <<  " readoutY " << readoutY <<  " readoutZ " << readoutZ << std::endl;
     
// Phase Enc. direction in 3D =  -0.540606 -0.827052 0.154046

     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     
    float phase_encX, phase_encY, phase_encZ;
    from >> phase_encX;
    from >> phase_encY;       
    from >> phase_encZ;
    std::cout << " phase_encX " << phase_encX <<  " phase_encY " << phase_encY <<  " phase_encZ " << phase_encZ << std::endl; 

// Slice select direction in 3D =  0.825469 -0.556809 -0.0925458
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     from >> str1;
     
    float slice_selX, slice_selY, slice_selZ;
    from >> slice_selX;
    from >> slice_selY;       
    from >> slice_selZ;
    std::cout << " slice_selX " << slice_selX <<  " slice_selY " << slice_selY <<  " slice_selZ " << slice_selZ << std::endl; 



// Skip 6 lines :
/*
The following are the (readout, phase enc, slice sel) coordinates (mm) of the grid points for which strains are calculated,
followed by their Ecc strain, an array of dimensions(NP, number of cine frames),
followed by their Err strain, an array of dimensions(NP, number of cine frames),
followed by their E11 strain, an array of dimensions(NP, number of cine frames),
followed by their E22 strain, an array of dimensions(NP, number of cine frames),
Note that RV Err, E11 and E22 strains are not calculated due to the small thickness.
*/
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------start skipping 1 line---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------stop skipping ---------------- " << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
std::cout << "------------stop skipping ---------------- " << std::endl;
  
   float *X = new float[NP];
   float *Y = new float[NP];   
   float *Z = new float[NP];
      
   char c;
   int i;   
   for (i=0; i<NP; i++) {
   
      from >> X[i];
      for (;;) {
        if (!from.get(c))
          break;
        if (!isspace(c)) {
          from.putback(c);
          break;
        }
     }  
      from >> Y[i];  
      for (;;) {
        if (!from.get(c))
          break;
        if (!isspace(c)) {
          from.putback(c);
          break;
        }
     }        
      from >> Z[i];              
   }

   std::cout << "--------------- Ecc_strain ------------------" << std::endl;
   float *ecc_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> ecc_strain[i]; 
       std::cout <<  ecc_strain[i] <<  std::endl;
   }

   std::cout << "--------------- Err_strain ------------------" << std::endl;
   float *err_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> err_strain[i]; 
       std::cout <<  err_strain[i] <<  std::endl;
   }
   
   std::cout << "--------------- E11_strain ------------------" << std::endl;
   float *e11_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> e11_strain[i]; 
       std::cout <<  e11_strain[i] <<  std::endl;
   }  
   
   std::cout << "--------------- E22_strain ------------------" << std::endl;
   float *e22_strain = new float[NP];
   for (i=0; i<NP; i++) {
       from >> e22_strain[i]; 
       std::cout <<  e22_strain[i] <<  std::endl;
   }    

   std::string dcmImageName;    

//followed by their Ecc strain, an array of NP elements,
   dcmImageName = textFileName + "_Ecc_strain.dcm";
   MakeDicomImage(ecc_strain, X, Y, Z, NP, dcmImageName);

//followed by their  Err strain, an array of NP elements,
   dcmImageName = textFileName + "_Err_strain.dcm";
   MakeDicomImage(err_strain, X, Y, Z, NP, dcmImageName);
   
//followed by their E11 strain, an array of NP elements,
   dcmImageName = textFileName + "_E11_strain.dcm";
   MakeDicomImage(e11_strain, X, Y, Z, NP, dcmImageName);
   
//followed by their E22 strain, an array of NP elements,
   dcmImageName = textFileName + "_E22_strain.dcm";
   MakeDicomImage(e22_strain, X, Y, Z, NP, dcmImageName);   
      
}


// =====================================================================================================================
    

void MakeDicomImage(float *tabVal, float *X, float *Y, float *Z, int NP, std::string dcmImageName)
{
   float minX = 999., minY = 999., minZ = 999.;
   float maxX = 0., maxY = 0., maxZ = 0.;
   int i;
   for (i=0; i<NP; i++) {
      // std::cout << X[i] << " " << Y[i] << " " << Z[i] <<  std::endl;
      if(maxX < X[i])
         maxX = X[i];
      if(maxY < Y[i])
         maxY = Y[i];
      if(maxZ < Z[i])
         maxZ = Z[i];
 
      if(minX > X[i])
         minX = X[i];
      if(minY > Y[i])
         minY = Y[i];
      if(minZ > Z[i])
         minZ = Z[i];
 
   }   
      std::cout << "Min X,Y,Z " << minX << " " << minY << " " << minZ <<  std::endl;
      std::cout << "Max X,Y,Z " << maxX << " " << maxY << " " << maxZ <<  std::endl;
      std::cout << "Size X,Y,Z " << maxX-minX << " " << maxY-minY << " " << maxZ-minZ <<  std::endl;      


    uint16_t *img = new uint16_t[int(maxX*maxY)];
    
    for(int i3=0;i3<int(maxX*maxY);i3++)
       img[i3] = 0; 
     
    for(int i2=0; i2<NP; i2++) {
    img[ int(maxX -(X[i2]-1))+  int((maxY -Y[i2])* maxX) ] = int(tabVal[i2]*100);
    }       
  
 // GDCM_NAME_SPACE::Debug::DebugOn();
  
       std::ostringstream str; 
       GDCM_NAME_SPACE::File *file;
       file = GDCM_NAME_SPACE::File::New();       
              
  // Set the image size
        str.str("");
        str << (int)maxX;
        file->InsertEntryString(str.str(),0x0028,0x0011,"US"); // Columns
        str.str("");
        str << (int)maxY;
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
    // cast is just to avoid warnings (*no* conversion)
    fileH->SetImageData((uint8_t *)img,int(maxX*maxY)*sizeof(uint16_t));
    fileH->SetWriteModeToRaw(); 
    fileH->SetWriteTypeToDcmExplVR();
        
    if( !fileH->Write(dcmImageName))
       std::cout << "Failed for [" << dcmImageName << "]\n"
                 << "           File is unwrittable" << std::endl;

    file->Print();
           
   delete img;
   file->Delete();
   fileH->Delete();  
}
