/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: TestDataEntry.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/18 08:35:46 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/
#include "gdcmDictEntry.h"
#include "gdcmDataEntry.h"

// ===============================================================

char data[]="1\\2\\3\\4\\5";
char fdata[]="1.1\\2.2\\3.3\\4.4\\5.5";

short svalue[]={1,2,3,4,5};
long lvalue[]={1,2,3,4,5};
float fvalue[]={1.1f,2.2f,3.3f,4.4f,5.5f};
double dvalue[]={1.1,2.2,3.3,4.4,5.5};

unsigned long nbvalue = 5;

/**
  * \brief Test the DataEntry object
  */  
int TestDataEntry(int argc, char *argv[])
{
   unsigned int error = 0;
   gdcm::DictEntry *dict;
   gdcm::DataEntry *entry;

   //------------------------------------------------------------------
   dict = new gdcm::DictEntry(0x0000,0x0000);
   entry = new gdcm::DataEntry(dict);
   dict->SetVR("US");

   std::cout << "Test for VR = " << dict->GetVR() << "..." << std::endl;

   entry->SetString("1");
   std::cout << "1: ";
   entry->Print(std::cout);
   std::cout << std::endl;
   if( entry->GetValueCount() != 1 )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }

   entry->SetString("1\\2");
   std::cout << "2: ";
   entry->Print(std::cout);
   std::cout << std::endl;
   if( entry->GetValueCount() != 2 )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }

   entry->SetString("");
   std::cout << "3: ";
   entry->Print(std::cout);
   std::cout << std::endl;
   if( entry->GetValueCount() != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }

   std::cout << std::endl;
   delete dict;
   delete entry;

   //------------------------------------------------------------------
   dict = new gdcm::DictEntry(0x0000,0x0000);
   entry = new gdcm::DataEntry(dict);
   dict->SetVR("LT");

   std::cout << "Test for VR = " << dict->GetVR() << "..." << std::endl;
   entry->SetString(data);
   entry->Print(std::cout);
   std::cout << std::endl;

   if( entry->GetLength() != strlen(data) + strlen(data)%2 )
   {
      std::cout << "   Failed" << std::endl
                << "   Size of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( memcmp(entry->GetBinArea(),data,entry->GetLength()) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of bin area is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( memcmp(entry->GetString().c_str(),data,entry->GetLength()) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != svalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl
                   << "   Found " << entry->GetValue(i) << " - Must be " << svalue[i] << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   std::cout << std::endl;
   delete dict;
   delete entry;

   //------------------------------------------------------------------
   dict = new gdcm::DictEntry(0x0000,0x0000);
   entry = new gdcm::DataEntry(dict);
   dict->SetVR("US");

   std::cout << "Test for VR = " << dict->GetVR() << "..." << std::endl;
   entry->SetString(data);
   std::cout << "1: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( entry->GetLength() != nbvalue*sizeof(uint16_t) )
   {
      std::cout << "   Failed" << std::endl
                << "   BinArea length is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( memcmp(entry->GetString().c_str(),data,strlen(data)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != svalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   entry->SetLength(nbvalue*sizeof(uint16_t));
   entry->SetBinArea((uint8_t *)svalue,false);
   std::cout << "2: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( memcmp(entry->GetString().c_str(),data,strlen(data)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != svalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   std::cout << std::endl;
   delete dict;
   delete entry;

   //------------------------------------------------------------------
   dict = new gdcm::DictEntry(0x0000,0x0000);
   entry = new gdcm::DataEntry(dict);
   dict->SetVR("UL");

   std::cout << "Test for VR = " << dict->GetVR() << "..." << std::endl;
   entry->SetString(data);
   std::cout << "1: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( entry->GetLength() != nbvalue*sizeof(uint32_t) )
   {
      std::cout << "   Failed" << std::endl
                << "   BinArea length is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( memcmp(entry->GetString().c_str(),data,strlen(data)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      std::cout<<"#"<<entry->GetString()<<"#"<<" / "<<"#"<<data<<"#"<<std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != lvalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   entry->SetLength(nbvalue*sizeof(uint32_t));
   entry->SetBinArea((uint8_t *)lvalue,false);
   std::cout << "2: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( memcmp(entry->GetString().c_str(),data,strlen(data)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != lvalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   std::cout << std::endl;
   delete dict;
   delete entry;

   //------------------------------------------------------------------
   dict = new gdcm::DictEntry(0x0000,0x0000);
   entry = new gdcm::DataEntry(dict);
   dict->SetVR("FL");

   std::cout << "Test for VR = " << dict->GetVR() << "..." << std::endl;
   entry->SetString(fdata);
   std::cout << "1: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( entry->GetLength() != nbvalue*sizeof(float) )
   {
      std::cout << "   Failed" << std::endl
                << "   BinArea length is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( memcmp(entry->GetString().c_str(),fdata,strlen(fdata)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != fvalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   entry->SetLength(nbvalue*sizeof(float));
   entry->SetBinArea((uint8_t *)fvalue,false);
   std::cout << "2: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( memcmp(entry->GetString().c_str(),fdata,strlen(fdata)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != fvalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   std::cout << std::endl;
   delete dict;
   delete entry;

   //------------------------------------------------------------------
   dict = new gdcm::DictEntry(0x0000,0x0000);
   entry = new gdcm::DataEntry(dict);
   dict->SetVR("FD");

   std::cout << "Test for VR = " << dict->GetVR() << "..." << std::endl;
   entry->SetString(fdata);
   std::cout << "1: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( entry->GetLength() != nbvalue*sizeof(double) )
   {
      std::cout << "   Failed" << std::endl
                << "   BinArea length is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( memcmp(entry->GetString().c_str(),fdata,strlen(fdata)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != dvalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   entry->SetLength(nbvalue*sizeof(double));
   entry->SetBinArea((uint8_t *)dvalue,false);
   std::cout << "2: ";
   entry->Print(std::cout);
   std::cout << std::endl;

   if( memcmp(entry->GetString().c_str(),fdata,strlen(fdata)) != 0 )
   {
      std::cout << "   Failed" << std::endl
                << "   Content of string is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   if( entry->GetValueCount() != nbvalue )
   {
      std::cout << "   Failed" << std::endl
                << "   Number of content values is incorrect" << std::endl;
      delete dict;
      delete entry;
      return(1);
   }
   for(unsigned int i=0;i<entry->GetValueCount();i++)
   {
      if( entry->GetValue(i) != dvalue[i] )
      {
         std::cout << "   Failed" << std::endl
                   << "   Content of entry's values is incorrect : id " << i << std::endl;
         delete dict;
         delete entry;
         return(1);
      }
   }

   std::cout << std::endl;
   delete dict;
   delete entry;

   //------------------------------------------------------------------
   std::cout<<std::flush;
   return 0;
}