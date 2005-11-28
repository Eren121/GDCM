/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmRefCounter.h,v $
  Language:  C++
  Date:      $Date: 2005/11/28 15:20:34 $
  Version:   $Revision: 1.9 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMREFCOUNTER_H
#define GDCMREFCOUNTER_H

#include "gdcmBase.h"

namespace gdcm 
{
//-----------------------------------------------------------------------------
/**
 * \brief Integration of reference counting with a destruction of the
 *        object only when the reference is to zero
 */
class GDCM_EXPORT RefCounter : public Base
{
   gdcmTypeMacro(RefCounter);

public:
// Allocator / Unallocator
   /// \brief Delete the object
   /// \remarks The object is deleted only if its reference counting is to zero
   void Delete() { Unregister(); }

// Reference count
   /// \brief Register the object
   /// \remarks It increments the reference counting
   void Register() { RefCount++; }

   /// \brief Unregister the object
   /// \remarks It decrements the reference counting
   void Unregister()
   {
      RefCount--;
      if(RefCount<=0)
         delete this;
   }
   /// \brief Get the reference counting
   /// \return Reference count
   const unsigned long &GetRefCount() const
   {
      return RefCount;
   }

protected:
   /// Constructor
   RefCounter() { RefCount = 1; }
   /// Destructor
   virtual ~RefCounter() {}

private:
   /// \brief Reference count
   unsigned long RefCount;
};
} // end namespace gdcm

//-----------------------------------------------------------------------------
#endif
