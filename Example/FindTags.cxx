#include <iostream>
#include "gdcm.h"
#include "math.h"
#include <stdio.h>

int main(int argc, char* argv[]) {  

   std::string toto, titi;

   gdcm::File  * f1;

   if(argc > 1 )
      f1 = new gdcm::File(argv[1]);
   else  {
      toto = GDCM_DATA_ROOT;
      toto += "/test.acr";
      f1 = new gdcm::File(toto);
   }

   std::string ManufacturerName="SIEMENS ";
   std::string RecCode="ACR-NEMA 2.0";
   std::string ImagePositionPatient, Location, ImageLocation;
   char zozo[100], zizi[50];
   char c;

   float x, y, z, l;

   int dataSize = f1->GetImageDataSize();
   printf ("---> pourFindTaggs : dataSize %d\n",dataSize);

   f1->SetEntryByNumber(RecCode ,0x0008,0x0010);
   f1->SetEntryByNumber(ManufacturerName ,0x0008,0x0070);

// ImagePositionPatient
   ImagePositionPatient = f1->GetHeader()->GetEntryByNumber(0x0020,0x0032);

// Image Position (RET)
   f1->SetEntryByNumber(ImagePositionPatient, 0x0020,0x0030);

   sscanf(ImagePositionPatient.c_str(), "%f%c%f%c%f", &x,&c,&y,&c,&z);

// ceci est probablement une mauvaise idée !
// (peut casser l'ordre des images si la pile d'images 
// traverse l'axe des X, ou des Y, ou des Z)
//l=sqrt(x*x + y*y + z*z);

// ceci ne marchera pas si on se déplace à Z constant :-(
   l=z;
// existerait-il qq chose qui marche à tout coup?

// Location
   sprintf(zizi,"%f\n",l);
   Location = zizi;
   f1->SetEntryByNumber(Location, 0x0020,0x0050);

// sinon, la longueur du champ est erronée (?!?) 
// Probable sac de noeud entre strlen(xxx.c_str()) et xxx.length()
// a eclaircir !

// SetEntryLengthByNumber is private now.
//TO DO : see is the pb goes on...

//f1->GetHeader()->SetEntryLengthByNumber(strlen(Location.c_str())-1, 0x0020,0x0050);

// Image Location 

   sprintf(zizi,"%d\n",0x7FE0);
   ImageLocation = zizi;
//f1->SetEntryByNumber(Location, 0x0028,0x0200);
//f1->GetHeader()->SetEntryLengthByNumber(strlen(ImageLocation.c_str())-1, 0x0020,0x0050); // prudence !

// void* imageData= f1->GetImageData();

// ecriture d'un fichier ACR à partir d'un dcmHeader correct.

   printf ("----------------avant PrintEntry---------------------\n");
   f1->GetHeader()->Print();
   printf ("----------------avant WriteDcm---------------------\n");


// ecriture d'un fichier ACR à partir d'un dcmHeader correct.

   sprintf(zozo, "%s.acr", toto.c_str());
   printf ("WriteACR\n");
   f1->WriteAcr(zozo);

   printf ("----------------apres Write---------------------\n");

  return 0;
}



