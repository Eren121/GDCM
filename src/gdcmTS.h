// gdcmTS.h

#ifndef GDCMTS_H
#define GDCMTS_H

#include <map>
#include <string>
#include "gdcmCommon.h"

typedef std::string TSKey;
typedef std::string TSAtr;
typedef std::map<TSKey, TSAtr> TSHT;    // Transfert Syntax Hash Table

/// Container for dicom Transfert Syntax Hash Table
/// \note   This is a singleton
class GDCM_EXPORT gdcmTS {
private:
   TSHT ts;	

public:
   gdcmTS(void);
   ~gdcmTS();
   int Count(TSKey key);
   std::string GetValue(TSKey key);
};

#endif
