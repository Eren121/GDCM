// gdcmTS.h
//-----------------------------------------------------------------------------
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
