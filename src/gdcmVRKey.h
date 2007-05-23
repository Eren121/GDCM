/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmVRKey.h,v $
  Language:  C++
  Date:      $Date: 2007/05/23 14:18:12 $
  Version:   $Revision: 1.7 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMVRKEY_H
#define GDCMVRKEY_H

#include "gdcmCommon.h"

#include <assert.h>
#include <iomanip> // important
#include <string>

namespace GDCM_NAME_SPACE 
{
//-----------------------------------------------------------------------------
class VRKey
{
public :
   inline VRKey() { key[0] = key[1] = ' ';}
   inline VRKey(const char *_key) { key[0] = _key[0]; key[1] = _key[1];}
   inline VRKey(const std::string &_key) { key[0] = _key[0]; key[1] = _key[1];}

   inline std::string str() const { return std::string(key,2); }

   friend std::ostream &operator<<(std::ostream &_os, const VRKey &_val);
   friend std::istream &operator>>(std::istream &_is, VRKey &_val);

   inline VRKey &operator=(const VRKey &_val)
   {
      key[0] = _val.key[0];
      key[1] = _val.key[1];
      return *this;
   }
   
   inline VRKey &operator=(const std::string &_val)
   {
      key[0] = _val[0];
      key[1] = _val[1];
      return *this;
   }
   
   inline VRKey &operator=(const char *_val)
   {
      key[0] = _val[0];
      key[1] = _val[1];
      return *this;
   }

   inline const char &operator[](const unsigned int &_id) const
   {
      assert(_id<2);
      return key[_id];
   }
   
   inline char &operator[](const unsigned int &_id)
   {
      assert(_id<2);
      return key[_id];
   }

   inline bool operator==(const VRKey &_val) const
   {
      return key[0] == _val.key[0] && key[1] == _val.key[1];
   }
   
   inline bool operator==(const std::string &_val) const
   {
      return key[0] == _val[0] && key[1] == _val[1];
   }
   
   inline bool operator==(const char *_val) const
   {
      return key[0] == _val[0] && key[1] == _val[1];
   }

   inline bool operator!=(const VRKey &_val) const
   {
      return key[0] != _val.key[0] || key[1] != _val.key[1];
   }
   
   inline bool operator!=(const std::string &_val) const
   {
      return key[0] != _val[0] || key[1] != _val[1];
   }
   inline bool operator!=(const char *_val) const
   {
      return key[0] != _val[0] || key[1] != _val[1];
   }

   inline bool operator<(const VRKey &_val) const
   {
      return key[0] < _val[0] || (key[0] == _val[0] && key[1] < _val[1]);
   }

private :
   char key[2];
};

//-----------------------------------------------------------------------------
inline std::ostream &operator<<(std::ostream &_os, const VRKey &_val)
{
   _os << _val.key[0] << _val.key[1];
   return _os;
}

inline std::istream &operator>>(std::istream &_is, VRKey &_val)
{
   _is >> _val.key[0] >> _val.key[1];
   return _is;
}

//-----------------------------------------------------------------------------

} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
