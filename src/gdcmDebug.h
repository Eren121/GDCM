/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmDebug.h,v $
  Language:  C++
  Date:      $Date: 2004/06/20 18:08:47 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMDEBUG_H
#define GDCMDEBUG_H

/**
 * \ingroup gdcmDebug
 * \brief gdcmDebug is an object for debugging in program.
 * It has 2 debugging modes :
 *  - error : for bad library use
 *  - debug : for debugging messages
 * 
 * A debugging message has a level of priority and is 
 * Shown only when the debug level is higher than the 
 * message level.
 */
class gdcmDebug {
public:
   gdcmDebug(int level = -1);

   void SetDebug (int level);
   void Verbose(int, const char*, const char* ="");
   void Error(bool, const char*,  const char* ="");
   void Error(const char*, const char* ="", const char* ="");

   void Assert(int, bool, const char*, const char*);
   void Exit(int);

private:
/// warning message level to be displayed
   int DebugLevel;
};

extern gdcmDebug dbg;

#endif
