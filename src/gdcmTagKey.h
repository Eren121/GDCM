/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTagKey.h,v $
  Language:  C++
  Date:      $Date: 2005/10/26 15:34:33 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMTAGKEY_H
#define GDCMTAGKEY_H

#include "gdcmCommon.h"

#include <assert.h>
#include <iostream>
#include <iomanip> // for std::ios::left, ...
#include <stdio.h> // for ugly sprintf

namespace gdcm 
{
//-----------------------------------------------------------------------------
class TagKey
{
public :
   TagKey(const uint16_t &gr, const uint16_t &elt) { tag[0] = gr;tag[1] = elt;}
   TagKey() { tag[0] = tag[1] = 0x0000;}

   friend std::ostream& operator<<(std::ostream& _os, const TagKey &_val);

   std::string str() const
   {
      char res[10];
      sprintf(res,"%04x|%04x",tag[0],tag[1]);
      return std::string(res);
   }

   void SetGroup(const uint16_t &group) { tag[0] = group; }
   uint16_t &GetGroup(void) { return tag[0]; } const

   void SetElement(const uint16_t &elem) { tag[1] = elem; }
   uint16_t GetElement(void) { return tag[1]; } const

   TagKey &operator=(const TagKey &_val)
   {
      tag[0] = _val.tag[0];
      tag[1] = _val.tag[1];
      return *this;
   }

   TagKey(const TagKey &_val)
     {
     tag[0] = _val[0];
     tag[1] = _val[1];
     }

   uint16_t &operator[](const unsigned int &_id) const
   {
      assert(_id<2);
      return tag[_id];
   }
   uint16_t &operator[](const unsigned int &_id)
   {
      assert(_id<2);
      return tag[_id];
   }

   bool operator==(const TagKey &_val) const
   {
      return tag[0] == _val.tag[0] && tag[1] == _val.tag[1];
   }

   bool operator!=(const TagKey &_val) const
   {
      return tag[0] != _val.tag[0] || tag[1] != _val.tag[1];
   }

   bool operator<(const TagKey &_val) const
   {
      return tag[0] < _val.tag[0] 
        || (tag[0] == _val.tag[0] && tag[1] < _val.tag[1]);
   }

private :
   uint16_t tag[2];
};

//-----------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& _os, const TagKey &_val)
{
   _os.setf( std::ios::right);
   _os << std::hex << std::setw( 4 ) << std::setfill( '0' )
       << _val.tag[0] << '|' << std::setw( 4 ) << std::setfill( '0' )
       << _val.tag[1] << std::setfill( ' ' ) << std::dec;
   return _os;
}

//-----------------------------------------------------------------------------

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
