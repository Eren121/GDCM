/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: FindTags.cxx,v $
  Language:  C++
  Date:      $Date: 2005/07/08 12:02:02 $
  Version:   $Revision: 1.14 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFileHelper.h"
#include "gdcmFile.h"
#include "gdcmUtil.h"

#include <iostream>
#include <stdio.h> // for sscanf

int main(int argc, char *argv[])
{
   std::string fileName;

   gdcm::FileHelper *h;
   gdcm::File *f = new gdcm::File();
   
   
   if(argc > 1 ) 
     f->SetFileName(argv[1]);
   else  
   {
      fileName = GDCM_DATA_ROOT;
      fileName += "/test.acr";
      f->SetFileName(fileName);
   }
   
   f->Load();
   // Should test if it worked !
   
   h = new gdcm::FileHelper(f);
   
   std::string ManufacturerName="SIEMENS ";
   std::string RecCode="ACR-NEMA 2.0";
   std::string ImagePositionPatient, Location, ImageLocation;
   std::string fileNameToWrite;
   char c;

   float x, y, z, l;

   int dataSize = h->GetImageDataSize();
   std::cout << "---> pourFindTaggs : dataSize " << dataSize << std::endl;

   h->SetValEntry(RecCode ,0x0008,0x0010);
   h->SetValEntry(ManufacturerName ,0x0008,0x0070);

// ImagePositionPatient
   ImagePositionPatient = h->GetFile()->GetEntryValue(0x0020,0x0032);

// Image Position (RET)
   h->SetValEntry(ImagePositionPatient, 0x0020,0x0030);

   sscanf(ImagePositionPatient.c_str(), "%f%c%f%c%f", &x,&c,&y,&c,&z);

// probablely a bad idea !
// (peut casser l'ordre des images si la pile d'images 
// traverse l'axe des X, ou des Y, ou des Z)
//l=sqrt(x*x + y*y + z*z);

// Will not work if we move on a Z constant :-(
   l=z;
// existerait-il qq chose qui marche à tout coup?

// Location
   std::string zizi = gdcm::Util::Format("%f",l);
   Location = gdcm::Util::DicomString(zizi.c_str());
   h->SetValEntry(Location, 0x0020,0x0050);

// sinon, la longueur du champ est erronée (?!?) 
// Probable sac de noeud entre strlen(xxx.c_str()) et xxx.length()
// a eclaircir !

// SetEntryLength is private now.
//TO DO : see if the pb goes on...

//h->GetFile()->SetEntryLength(strlen(Location.c_str())-1, 0x0020,0x0050);

// Image Location 

   zizi = gdcm::Util::Format("%d",0x7FE0);
   ImageLocation = gdcm::Util::DicomString(zizi.c_str());
//h->SetValEntry(Location, 0x0028,0x0200);
//h->GetFile()->SetEntryLength(strlen(ImageLocation.c_str())-1, 0x0020,0x0050); // prudence !

// void *imageData= h->GetImageData();

// ecriture d'un fichier ACR à partir d'un dcmFile correct.

   std::cout << "----------------before PrintEntry---------------------" << std::endl;
   h->GetFile()->Print();
   std::cout << "----------------before WriteDcm---------------------" << std::endl;


// ecriture d'un fichier ACR à partir d'un dcmFile correct.

   fileNameToWrite = fileName + ".acr";
   std::cout << "WriteACR" << std::endl;
   h->WriteAcr(fileNameToWrite);

   std::cout << "----------------apres Write---------------------" << std::endl;

   return 0;
}



