/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTS.h,v $
  Language:  C++
  Date:      $Date: 2004/09/27 08:39:08 $
  Version:   $Revision: 1.8 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMTS_H
#define GDCMTS_H

#include "gdcmCommon.h"
#include <map>
#include <string>
#include <iostream>

//-----------------------------------------------------------------------------
typedef std::string TSKey;
typedef std::string TSAtr;
typedef std::map<TSKey, TSAtr> TSHT;    // Transfert Syntax Hash Table

//-----------------------------------------------------------------------------
/*
 * Container for dicom Transfert Syntax Hash Table
 * \note   This is a singleton
 */
class GDCM_EXPORT gdcmTS {
public:
   gdcmTS(void);
   ~gdcmTS();

   void Print(std::ostream &os = std::cout);

   int Count(TSKey key);
   std::string GetValue(TSKey key);

private:
   TSHT ts;
};

//-----------------------------------------------------------------------------
#endif
