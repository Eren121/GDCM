/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmException.cxx,v $
  Language:  C++
  Date:      $Date: 2004/10/08 04:52:55 $
  Version:   $Revision: 1.19 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmException.h"

#include <typeinfo>

//-----------------------------------------------------------------------------
// gdcmException

/**
 * \ingroup gdcmException
 * \brief constructor
 * @param f
 * @param msg  
 */
gdcmException::gdcmException(const std::string &f, const std::string& msg) throw()
#ifdef __GNUC__
  try
#endif
  : From(f), Error(msg) {
  }
#ifdef __GNUC__
catch(...) {
  fatal("gdcmException::gdcmException(const std::string&, const std::string&, const std::string&)");
}
#endif


/**
 * \ingroup gdcmException
 * \brief fatal
 * @param from 
 */
void gdcmException::fatal(const char *from) throw() {
   try
   {
      std::cerr << "Fatal: exception received in " << from 
                << " while handling exception." << std::endl;
      exit(-1);
   }
   catch(...)
   {
      try
      {
         std::cerr << "Fatal: exception received in Exception::fatal while handling exception."
                   << std::endl;
         exit(-1);
      }
      catch(...)
      {
         exit(-1);
      }
   }
}

/**
 * \ingroup gdcmException
 * \brief getName
 * @return string
 */
std::string gdcmException::getName() const throw()
{
   try
   {
#ifdef __GNUC__   // GNU C++ compiler class name demangling
      unsigned int nested = 1, i, nb, offset;
      std::string one;

      std::string name;
      std::string iname = typeid(*this).name();
      if(iname[0] == 'Q')
      {
         nested = iname[1] - '0';
         iname = std::string(iname, 2, std::string::npos);
      }
      for(i = 0; i < nested; i++)
      {
         ::sscanf(iname.c_str(), "%u%n", &nb, &offset);
         iname = std::string(iname, offset, std::string::npos);
         name += std::string(iname, 0, nb);
         if(i + 1 < nested) name += "::";
         iname = std::string(iname, nb, std::string::npos);
      }
      return name;
#else           // no class name demangling
      //name = typeid(*this).name();
      return "Exception";
#endif
  }
  catch(...) {
    fatal("Exception::getName(std::string &)");
    return "";
  }
}

/**
 * \ingroup gdcmException
 * \brief gdcmException
 */
 gdcmException::operator const char *() const throw() {
  return getName().c_str();
}

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmException
 * \brief gdcmException::operator <<
 */
 std::ostream& operator<<(std::ostream &os, const gdcmException &e) {
  try {  
    os << "Exception " << e.getName() << " thrown: " << e.getError() << std::endl;
  }
  catch(...) {
    gdcmException::fatal("operator<<(std::ostream &, const gdcmException&)");
  }
  return os;
}

//-----------------------------------------------------------------------------
