/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/27 11:55:57 $
  Version:   $Revision: 1.18 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmDebug.h"
#include <iostream>

namespace gdcm 
{

/// warning message level to be displayed
static bool DebugFlag   = false;
static bool DebugToFile = false;
static std::ofstream DebugFile;


Debug::Debug()
{

}

Debug::~Debug()
{
  if ( DebugFile.is_open() )
      DebugFile.close();     
}
//-----------------------------------------------------------------------------
/**
 * \brief   Accessor
 * @param   flag Set the debug flag
 */ 
void Debug::SetDebugFlag (bool flag) 
{
   DebugFlag = flag;
}

/**
 * \brief   Accessor
 * @param   level Get the debug flag
 */ 
bool Debug::GetDebugFlag ()
{
   return DebugFlag;
}

/**
 * \brief   Accessor
 * @param   flag Set the debug flag to redirect to file
 */ 
void Debug::SetDebugToFile (bool flag) 
{
   DebugToFile = flag;
}

/**
 * \brief   Accessor
 * @param   level Get the debug flag to redirect to file
 */ 
bool Debug::GetDebugToFile ()
{
   return DebugToFile;
}

/**
 * \brief   Accessor
 * @param   flag Set the debug flag to redirect to file
 *          Absolutely nothing is check. You have to pass in
 *          a correct filename
 */ 
void Debug::SetDebugFilename (std::string const &filename)
{
   DebugToFile = true;  // Just in case ... 
   DebugFlag = true;    // Just in case ...
   if( DebugFile.is_open() )
      DebugFile.close();
   DebugFile.open( filename.c_str() );
}

std::ofstream &Debug::GetDebugFile ()
{
  return DebugFile;
}

} // end namespace gdcm


