/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmPixelWriteConvert.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/06 20:03:28 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

//////////////////   TEMPORARY NOTE
// look for "fixMem" and convert that to a member of this class
// Removing the prefix fixMem and dealing with allocations should do the trick
//
// grep PixelWriteConvert everywhere and clean up !

#include "gdcmDebug.h"
#include "gdcmPixelWriteConvert.h"
#include <stdio.h>

namespace gdcm
{
//-----------------------------------------------------------------------------
// Constructor / Destructor
PixelWriteConvert::PixelWriteConvert() 
{
   ReadData = 0;
   ReadDataSize = 0;

   UserData = 0;
   UserDataSize = 0;
}

PixelWriteConvert::~PixelWriteConvert() 
{
}

//-----------------------------------------------------------------------------
// Public
void PixelWriteConvert::SetReadData(uint8_t *data,size_t size)
{
   ReadData = data;
   ReadDataSize = size;
}

void PixelWriteConvert::SetUserData(uint8_t *data,size_t size)
{
   UserData = data;
   UserDataSize = size;
}

uint8_t *PixelWriteConvert::GetData()
{
   if(UserData)
   {
      return UserData;
   }
   else
   {
      return ReadData;
   }
}

size_t PixelWriteConvert::GetDataSize()
{
   if(UserData)
   {
      return UserDataSize;
   }
   else
   {
      return ReadDataSize;
   }
}

//-----------------------------------------------------------------------------
} // end namespace gdcm
