/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: FindTags.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/20 16:16:58 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmFileHelper.h"
#include "gdcmHeader.h"
#include "gdcmUtil.h"

#include <iostream>
#include <stdio.h> // for sscanf

int main(int argc, char* argv[])
{
   std::string toto, titi;

   gdcm::FileHelper *f1;

   if(argc > 1 )
      f1 = new gdcm::FileHelper(argv[1]);
   else  {
      toto = GDCM_DATA_ROOT;
      toto += "/test.acr";
      f1 = new gdcm::FileHelper(toto);
   }

   std::string ManufacturerName="SIEMENS ";
   std::string RecCode="ACR-NEMA 2.0";
   std::string ImagePositionPatient, Location, ImageLocation;
   std::string zozo;
   char c;

   float x, y, z, l;

   int dataSize = f1->GetImageDataSize();
   std::cout << "---> pourFindTaggs : dataSize " << dataSize << std::endl;

   f1->SetEntry(RecCode ,0x0008,0x0010);
   f1->SetEntry(ManufacturerName ,0x0008,0x0070);

// ImagePositionPatient
   ImagePositionPatient = f1->GetHeader()->GetEntry(0x0020,0x0032);

// Image Position (RET)
   f1->SetEntry(ImagePositionPatient, 0x0020,0x0030);

   sscanf(ImagePositionPatient.c_str(), "%f%c%f%c%f", &x,&c,&y,&c,&z);

// ceci est probablement une mauvaise idée !
// (peut casser l'ordre des images si la pile d'images 
// traverse l'axe des X, ou des Y, ou des Z)
//l=sqrt(x*x + y*y + z*z);

// ceci ne marchera pas si on se déplace à Z constant :-(
   l=z;
// existerait-il qq chose qui marche à tout coup?

// Location
   std::string zizi = gdcm::Util::Format("%f",l);
   Location = gdcm::Util::DicomString(zizi.c_str());
   f1->SetEntry(Location, 0x0020,0x0050);

// sinon, la longueur du champ est erronée (?!?) 
// Probable sac de noeud entre strlen(xxx.c_str()) et xxx.length()
// a eclaircir !

// SetEntryLength is private now.
//TO DO : see is the pb goes on...

//f1->GetHeader()->SetEntryLength(strlen(Location.c_str())-1, 0x0020,0x0050);

// Image Location 

   zizi = gdcm::Util::Format("%d",0x7FE0);
   ImageLocation = gdcm::Util::DicomString(zizi.c_str());
//f1->SetEntry(Location, 0x0028,0x0200);
//f1->GetHeader()->SetEntryLength(strlen(ImageLocation.c_str())-1, 0x0020,0x0050); // prudence !

// void* imageData= f1->GetImageData();

// ecriture d'un fichier ACR à partir d'un dcmHeader correct.

   std::cout << "----------------avant PrintEntry---------------------" << std::endl;
   f1->GetHeader()->Print();
   std::cout << "----------------avant WriteDcm---------------------" << std::endl;


// ecriture d'un fichier ACR à partir d'un dcmHeader correct.

   zozo = toto + ".acr";
   std::cout << "WriteACR" << std::endl;
   f1->WriteAcr(zozo);

   std::cout << "----------------apres Write---------------------" << std::endl;

   return 0;
}



