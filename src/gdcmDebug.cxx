/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.cxx,v $
  Language:  C++
  Date:      $Date: 2005/11/05 13:21:32 $
  Version:   $Revision: 1.26 $
                                                                                
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
//-----------------------------------------------------------------------------
// Warning message level to be displayed
static bool DebugFlag     = false;
static bool WarningFlag   = false;
static bool DebugToFile   = false;
static std::ofstream DebugFile;

//-----------------------------------------------------------------------------
// Constructor / Destructor
Debug::Debug()
{

}

Debug::~Debug()
{
  if ( DebugFile.is_open() )
      DebugFile.close();     
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief   Sets both the debug flag and warning flag
 *          (both used for debugging purpose)
 * @param   flag Set the debug flag and warning flag
 */ 
void Debug::SetDebugFlag (bool flag) 
{
   // To help tracking a bug, both flags are necessary
   DebugFlag   = flag;
   WarningFlag = flag;
}

/**
 * \brief   Gets the debug flag value
 *          (used to warn user when file contains some oddity)
 * @return debug flag value
 */ 
bool Debug::GetDebugFlag ()
{
   return DebugFlag;
}

/**
 * \brief   Sets the warning flag
 * @param   flag Set the warning flag
 */ 
void Debug::SetWarningFlag (bool flag) 
{
   // Cannot unset Warning flag if Debug flag is on.
   if (flag == false && DebugFlag == true)
      return;
   WarningFlag = flag;
}

/**
 * \brief   Gets the warning flag value
 * @return warning flag value
 */ 
bool Debug::GetWarningFlag ()
{
   return WarningFlag;
}
/**
 * \brief   Accessor
 * @param   flag whether we want to redirect to file
 */ 
void Debug::SetDebugToFile (bool flag) 
{
   DebugToFile = flag;
}

/**
 * \brief   Accessor to know whether debug info are redirected to file
 */ 
bool Debug::GetDebugToFile ()
{
   return DebugToFile;
}

/**
 * \brief Set the filename the debug stream should be redirect to
 *        Settting a filename also sets DebugToFile to true
 * @param   filename  File to redirect debug info
 *          Absolutely nothing is check. You have to pass in
 *          a correct filename
 */ 
void Debug::SetDebugFilename (std::string const &filename)
{
   DebugToFile = true;  // Just in case ... 
   DebugFlag   = true;  // Just in case ...
   if ( DebugFile.is_open() )
      DebugFile.close();
   DebugFile.open( filename.c_str() );
}

/**
 * \brief Internal use only. Allow us to retrieve the static from anywhere
 *        in gdcm code
 * @return Debug file
 */
std::ofstream &Debug::GetDebugFile ()
{
  return DebugFile;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
} // end namespace gdcm
