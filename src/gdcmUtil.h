/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmUtil.h,v $
  Language:  C++
  Date:      $Date: 2005/11/29 13:02:47 $
  Version:   $Revision: 1.66 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include "gdcmCommon.h"
#include <vector>
#include <string>

namespace gdcm 
{
/**
 * \brief    Here are some utility functions, belonging to the gdcm::Util class,
 *           dealing with strings, file names... that can be called
 *           from anywhere by whomsoever they can help.
 */

//-----------------------------------------------------------------------------

class GDCM_EXPORT Util
{
public:
   static std::string Format(const char *format, ...);
   static void        Tokenize (const std::string &str,
                                std::vector<std::string> &tokens,
                                const std::string &delimiters = " ");
   static int         CountSubstring (const std::string &str,
                                      const std::string &subStr);

   static std::string CreateCleanString(std::string const &s);
   static std::string CreateCleanString(uint8_t *s, int l);
   static bool IsCleanString(std::string const &s);
   static bool IsCleanArea(uint8_t *s, int l);
   static std::string NormalizePath(std::string const &name);
   static std::string GetPath(std::string const &fullName);
   static std::string GetName(std::string const &fullName);
   static std::string GetCurrentDate();
   static std::string GetCurrentTime();
   static std::string GetCurrentDateTime();
   /// Provides a simple static GetVersion() function
   static std::string GetVersion() 
                      { return GDCM_VERSION;}
   static unsigned int GetCurrentThreadID();
   static unsigned int GetCurrentProcessID();
   static bool         IsCurrentProcessorBigEndian();

   static std::string DicomString(const char *s, size_t l);
   static std::string DicomString(const char *s);
   static bool        DicomStringEqual(const std::string &s1, const char *s2);
   static bool        CompareDicomString(const std::string &s1, 
                                         const char *s2, int op);
   static std::string GetMACAddress();

   static std::string CreateUniqueUID(const std::string &root = "");
   static void SetRootUID(const std::string &root = "");
   static const std::string &GetRootUID();

   static const uint8_t *GetFileMetaInformationVersion() 
                     { return FileMetaInformationVersion;}
   static void SetFileMetaInformationVersion( uint16_t fmiv )
                     { FileMetaInformationVersion = (uint8_t *)&fmiv; }

// ----------------- to be removed later --------------------------     
//To perform a full check of inline functions on all the platforms, 
// we put here the two 'static' functions
// hifpswap and  hNoifpswap in a .h
// They will be remove ASAP

   inline void hifpswap(double *a, double *b)     
   {
      double tmp;
      tmp = *a;
      *a = *b;
      *b = tmp;
   }
   void hNoifpswap(double *a, double *b)    
   {
      double tmp;
      tmp = *a;
      *a = *b;
      *b = tmp;
   }    
   void hfpswap(double *a, double *b);   
   static inline void sthifpswap(double *a, double *b)     
   {
      double tmp;
      tmp = *a;
      *a = *b;
      *b = tmp;
   }
   static void sthNoifpswap(double *a, double *b)    
   {
      double tmp;
      tmp = *a;
      *a = *b;
      *b = tmp;
   }    
// ------------ end of functions to remove --------------------
private:
   static std::string GetIPAddress(); //Do not expose this method

   static std::string RootUID;
   static const std::string GDCM_UID;
   static uint8_t *FileMetaInformationVersion;
   static const uint16_t FMIV;
   static std::string GDCM_MAC_ADRESS;
};

GDCM_EXPORT std::ostream &binary_write(std::ostream &os, const uint16_t &val);
GDCM_EXPORT std::ostream &binary_write(std::ostream &os, const uint32_t &val);
GDCM_EXPORT std::ostream &binary_write(std::ostream &os, const double &val);
GDCM_EXPORT std::ostream &binary_write(std::ostream &os, const char *val);
GDCM_EXPORT std::ostream &binary_write(std::ostream &os, std::string const &val);
GDCM_EXPORT std::ostream &binary_write(std::ostream &os, const uint8_t *val, size_t len);
GDCM_EXPORT std::ostream &binary_write(std::ostream &os, const uint16_t *val, size_t len);
} // end namespace gdcm
//-----------------------------------------------------------------------------
#endif
