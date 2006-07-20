/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: DenseMultiFramesToDicom.cxx,v $
  Language:  C++
  Date:      $Date: 2006/07/20 17:15:28 $
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
#include "gdcmUtil.h"

#include "gdcmArgMgr.h"

/**
  * \brief   
  *          - explores recursively the (single Patient, single Study) directory
  *          - examines the ".txt" Dense multiframe files 
  *          - Converts the files into 16 bits Dicom Files
  *           WARNING : directory must contain ONLY .txt files 
  */  


void Load(std::ifstream &from, std::string imageName, std::string patName, 
          std::string strStudyUID, int serieNumber);

//std::ifstream& eatwhite(std::ifstream& is);

/*
window center (level) and width, are defined in the DICOM 
standard very precisely as follows (see PS 3.3 C.11.2.1.2):
>
>"These Attributes are applied according to the following pseudo-code, 
>where :
x is the input value, 
y is an output value with a range from ymin to ymax, 
c is Window Center (0028,1050)
w is Window Width (0028,1051):
>
>           if      (x  <= c - 0.5 - (w-1)/2), then y = ymin
>           else if (x > c - 0.5 + (w-1)/2), then y = ymax,
>           else    y = ((x - (c - 0.5)) / (w-1) + 0.5) * (ymax - ymin)+ ymin

*/
/*
From:   David Clunie - view profile
Date:   Thurs, Jun 1 2006 3:03 pm
Email:  David Clunie <dclu...@dclunie.com>
Groups: comp.protocols.dicom

The value of x is the output of the preceding step, the so-called
"modality LUT", which may either be:

- identity (no rescale values or Modality LUT, or the SOP Class is
  PET and rescale values are ignored), in which case x is the stored
  pixel value in (7FE0,0010)

- Rescale Slope and Intercept (as typically used in CT), in which
  case x is the value obtained from applying the rescale values to
  the stored pixel value

- an actual LUT, in which case x is the value stored in the LUT
  corresponding to the LUT index value that is the stored pixel
  value

The ymin and ymax are intended to represent the output range; for
example, if the hypothetical Presentation LUT step that follows
the VOI LUT (window) stage is an identity operation, then the
ymin and ymax represent P-Values, which might be the range of
digital driving levels for your display (calibrated to the GSDF),
in the 8-bit wide output case ranging from 0 to 255, for example.

The terms brightness and contrast are not used in radiology imaging 
-the window center and width are used instead. 
*/

int main(int argc, char *argv[])
{
   START_USAGE(usage)
   " \n DenseMultiframeToDicom :\n                                            ",
   " - explores recursively the given (single Patient, single Study) directory",
   "         - examines the '.txt' files                                      ",
   "         - Converts the files into 16 bits Dicom files,                   ",
   " WARNING : directory must contain ONLY .txt files                         ",
   " usage:                                                                   ",
   " DenseMultiframeToDicom dirin=rootDirectoryName                           ",
   "              [studyUID = ] [patName = ]                                  ",
   "              [listonly] [verbose] [debug]                                ",
   "                                                                          ",
   "studyUID   : *aware* user wants to add the serie                          ",
   "                                             to an already existing study ",
   " verbose  : user wants to run the program in 'verbose mode'               ",
   " debug    : *developer*  wants to run the program in 'debug mode'         ",
   FINISH_USAGE

   // ----- Initialize Arguments Manager ------
      
   gdcm::ArgMgr *am = new gdcm::ArgMgr(argc, argv);
  
   if (argc == 1 || am->ArgMgrDefined("usage")) 
   {
      am->ArgMgrUsage(usage); // Display 'usage'
      delete am;
      return 0;
   }

   const char *dirNamein;   
   dirNamein  = am->ArgMgrGetString("dirin","."); 

   if (am->ArgMgrDefined("debug"))
      gdcm::Debug::DebugOn();
      
   int verbose  = am->ArgMgrDefined("verbose");      
   int listonly = am->ArgMgrDefined("listonly");
   std::string patName = am->ArgMgrGetString("patname", "g^PatientName");

   bool userDefinedStudy = am->ArgMgrDefined("studyUID");
   const char *studyUID  = am->ArgMgrGetString("studyUID");

// not described *on purpose* in the Usage ! 
   bool userDefinedSerie = am->ArgMgrDefined("serieUID");   
   const char *serieUID  = am->ArgMgrGetString("serieUID");
      
      
   // if unused Param we give up
   if ( am->ArgMgrPrintUnusedLabels() )
   { 
      am->ArgMgrUsage(usage);
      delete am;
      return 0;
   }
   delete am;  // we don't need Argument Manager any longer

   // ----- Begin Processing -----
   
   if ( ! gdcm::DirList::IsDirectory(dirNamein) )
   {
      std::cout << "KO : [" << dirNamein << "] is not a Directory." 
                << std::endl;
      return 0;
   }
   else
   {
      std::cout << "OK : [" << dirNamein << "] is a Directory." << std::endl;
   }

   std::string strDirNamein(dirNamein);
   gdcm::DirList dirList(strDirNamein, true); // (recursively) the list of files

   if (listonly)
   {
      std::cout << "------------List of found files ------------" << std::endl;
      dirList.Print();
      std::cout << std::endl;
      return 0;
    }
   
   std::string filenameout;

   std::string strStudyUID;
   strStudyUID =  gdcm::Util::CreateUniqueUID();
   int serieNumber =0;     
   gdcm::DirListType fileNames;
   fileNames = dirList.GetFilenames();
   for (gdcm::DirListType::iterator it = fileNames.begin();  
                                    it != fileNames.end();
                                  ++it)
   {
   
   
      std::ifstream from( (*it).c_str() );   
      if ( !from )
      {
         std::cout << "Can't open file" << *it << std::endl;
         //return 0;
      }
      else
      { 
         std::cout << "Success in open file" << *it << std::endl;
         Load(from, *it, patName, strStudyUID, serieNumber);
         serieNumber++;
         //return 0;
      }   
   }
}


void Load(std::ifstream &from, std::string imageName, std::string patName, 
          std::string strStudyUID, int serieNumber)
{
   if (!from)
      return;

/* was OK for single frames
eg :
---------------------------
 Array dimensions = 58 x 56
The following is the 2D array of peak circumferential strain map, all zero value
pixels are outside the mask
     0.000000     0.000000     0.000000     0.000000     0.000000     0.000000  
-----------------------------
   std::string str1;
   int nx, ny;
   // Get nx, ny   
   from >> str1;
   from >> str1;
   from >> str1;
   from >> nx;
   from >> str1;      
   from >> ny;
   
   std::cout << "nx " << nx << " ny " << ny << std::endl;
   
   // Skip 2 lines.
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
   std::getline(from, str1);
   std::cout << "[" << str1 << "]" << std::endl;
 */
 
 /* now, we deal with multiframes
 eg :
 ------------------------------------------
X dim, Y dim, N of frames = 52x59x14
Temporal resolution = 31.9200 ms
First frame starts at 47.9600 ms
The following are the 3D arrays of peak circumferential strain map and the magnitude images,
All pixels with zero strain values are outside the masks.
     0.000000     0.000000     0.000000     0.000000     0.000000     0.000000
--------------------------------------------
*/
   std::string str1;
   int nx, ny, nf;
    from >> str1; // X dim,
    from >> str1; 
    from >> str1; // Y dim,
    from >> str1;   
    from >> str1; // N of frames =     
    from >> str1;
    from >> str1;
    from >> str1;
    from >> str1; // 52x59x14
   
    std::cout << "[" << str1 << "]" << std::endl;
    
    sscanf( str1.c_str(),"%dx%dx%d", &nx,&ny,&nf);
    std::cout << nx << " " << ny << " " << nf << std::endl;
    
       // Skip 4 lines.
    for (int k=0; k<4; k++)
    {   
       std::getline(from, str1);
       std::cout << str1 << std::endl;
    }        
             
   //float *f = new float(nx*ny);
   float *f = (float *) malloc(nx*ny*nf*sizeof(float));
  // float mini = FLT_MAX, maxi = FLT_MIN;
   float val;
 
    
   std::string strSerieUID;
   strSerieUID =  gdcm::Util::CreateUniqueUID();     
   
  for (int l=0; l<nf; l++)  // Loop on the frames
  { 
     for( int j=0; j<ny; j++)
     { 
      int l =0;   
      for (int i=0; i<nx; i++)
      {
         //eatwhite(from);
         char c;
         for (;;)
         {
            if (!from.get(c))
               break;
            if (!isspace(c)) 
            {
               from.putback(c);
               break;
            }
         }  
         from >> str1;
         val = (float)atof(str1.c_str());
        // std::cout << "  " << val;
         *(f+ /*l*nx*ny + */j*nx+i) = val;
 
        if(from.eof()) 
        {
            std::cout << "Missing values at [" << j <<"," << i << "]" 
                      << std::endl; 
           break;
         }
         l++;           
      }
      
       std::cout << std::endl;
      //std::cout << std::endl << " line nb : " << j 
      //          << " line length : " << l << std::endl;
         
    }
   
    
   // std::cout << "mini : "<< mini  << " maxi : " << maxi << std::endl;
/*
// values are expressed as % as a fraction, actually!)
// It's useless to rescale them as uint16_t : just *100
    uint16_t *img = new uint16_t[ny*nx];
    uint16_t *ptr = img;
    float *tmp = f;
    float div = maxi-mini;
    std::cout << "div : " << div << " mini : " << mini << std::endl;
    for( int k=0; k<ny*nx; k++)
    {
       *ptr = (uint16_t)((*tmp * 65535.0) / div);
       tmp ++;
       ptr++;
    }     
*/
    uint16_t *img = new uint16_t[ny*nx];
    uint16_t *ptr = img;
    float *tmp = f /* + l*ny*nx */ ; // start on image nb l.
    for( int k=0; k<ny*nx; k++)
    {
       if(*tmp < 0) // artefacted pixel
          *ptr = 0;
       else        /// \todo FIXME : what about max threshold ?
        *ptr = (int16_t)(*tmp *100); 

       //std::cout << std::dec << "[" << *tmp <<" : " << *ptr << "] ";
       tmp ++;
       ptr++;
    }  

 // gdcm::Debug::DebugOn();
  
        std::ostringstream str; 
        gdcm::File *file;
        file = gdcm::File::New();       
              
  // Set the image size
        str.str("");
        str << nx;
        file->InsertEntryString(str.str(),0x0028,0x0011,"US"); // Columns
        str.str("");
        str << ny;
        file->InsertEntryString(str.str(),0x0028,0x0010,"US"); // Rows

  // Set the pixel type
  //      16; //8, 16, 32
        file->InsertEntryString("16",0x0028,0x0100,"US"); // Bits Allocated
        str.str("");
        str << 16; // may be 12 or 16 if componentSize =16
        file->InsertEntryString("16",0x0028,0x0101,"US"); // Bits Stored

        file->InsertEntryString("15",0x0028,0x0102,"US"); // High Bit

  // Set the pixel representation // 0/1 1 : signed
        file->InsertEntryString("0",0x0028,0x0103, "US"); // Pixel Representation

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

// 0020 0037 DS 6 Image Orientation (Patient)
         file->InsertEntryString("1.0\\0.0\\0.0\\0.0\\1.0\\0.0",0x0020,0x0037, "DS"); //[1\0\0\0\1\0] : Axial

// 0020 0032 DS 3 Image Position (Patient)
        char charImagePosition[256]; 
        sprintf(charImagePosition,"0.0\\0.0\\%f",float(l%nf));
 
// 0020 0x1041 DS 1 Slice Location 
        sprintf(charImagePosition,"%f",float(l%nf));
        file->InsertEntryString(charImagePosition,0x0020,0x1041, "DS");
 
//0008 103e LO 1 Series Description
        file->InsertEntryString(imageName,0x0008,0x103e, "LO");

        file->InsertEntryString(strStudyUID,0x0020,0x000d,"UI");      
        file->InsertEntryString(strSerieUID,0x0020,0x000e,"UI");
        file->InsertEntryString(patName,0x0010,0x0010, "PN");   // Patient's Name 
      
//0020 0011 "IS" Series Number 
         sprintf(charImagePosition,"%04d",serieNumber);
         file->InsertEntryString(charImagePosition,0x0020,0x0011, "IS");

   // file->Print();
    
    gdcm::FileHelper *fh;
    fh = gdcm::FileHelper::New(file);
    // cast is just to avoid warnings (*no* conversion)
    fh->SetImageData((uint8_t *)img,nx*ny*sizeof(uint16_t));
    fh->SetWriteModeToRaw(); 
    fh->SetWriteTypeToDcmExplVR();
    
    fh->SetWriteTypeToDcmExplVR();


    char numero[10];
    sprintf(numero, "%02d", l);   
    std::string fileName = imageName + "." + numero + ".dcm";
    std::cout << "fileName " << fileName << std::endl;
      
    if( !fh->Write(fileName))
       std::cout << "Failed for [" << fileName << "]\n"
                 << "           File is unwrittable" << std::endl;

    delete img; 
  } // end loop on frames
       
   from.close();
}
