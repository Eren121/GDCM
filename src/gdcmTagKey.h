/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmTagKey.h,v $
  Language:  C++
  Date:      $Date: 2005/10/20 09:23:24 $
  Version:   $Revision: 1.2 $
                                                                                
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

namespace gdcm 
{
//-----------------------------------------------------------------------------
class TagKey
{
public :
   inline TagKey(const uint16_t &gr, const uint16_t &elt) { tag[0] = gr;tag[1] = elt;}
   inline TagKey() { tag[0] = tag[1] = 0x0000;}

   friend std::ostream& operator<<(std::ostream& _os, const TagKey &_val);

   inline std::string str() const
   {
      char res[10];
      sprintf(res,"%04x|%04x",tag[0],tag[1]);
      return std::string(res);
   }

   inline void SetGroup(const uint16_t &val) { tag[0] = val; }
   inline const uint16_t &GetGroup(void) { return tag[0]; }

   inline void SetElement(const uint16_t &val) { tag[1] = val; }
   inline const uint16_t &GetElement(void) { return tag[1]; }

   inline TagKey &operator=(const TagKey &_val)
   {
      tag[0] = _val.tag[0];
      tag[1] = _val.tag[1];
      return *this;
   }

   inline const uint16_t &operator[](const unsigned int &_id) const
   {
      assert(_id<2);
      return tag[_id];
   }
   inline const uint16_t &operator[](const unsigned int &_id)
   {
      assert(_id<2);
      return tag[_id];
   }

   inline bool operator==(const TagKey &_val) const
   {
      return tag[0] == _val.tag[0] && tag[1] == _val.tag[1];
   }

   inline bool operator!=(const TagKey &_val) const
   {
      return tag[0] != _val.tag[0] || tag[1] != _val.tag[1];
   }

   inline bool operator<(const TagKey &_val) const
   {
      return tag[0] < _val.tag[0] || (tag[0] == _val.tag[0] && tag[1] < _val.tag[1]);
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
