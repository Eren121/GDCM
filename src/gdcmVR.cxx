// gdcmVR.cxx

#include "gdcmVR.h"

gdcmVR::gdcmVR(void) {
   vr["AE"] = "Application Entity";    // At most 16 bytes
   vr["AS"] = "Age String";            // Exactly 4 bytes
   vr["AT"] = "Attribute Tag";         // 2 16-bit unsigned short integers
   vr["CS"] = "Code String";           // At most 16 bytes
   vr["DA"] = "Date";                  // Exactly 8 bytes
   vr["DS"] = "Decimal String";        // At most 16 bytes
   vr["DT"] = "Date Time";             // At most 26 bytes
   vr["FL"] = "Floating Point Single"; // 32-bit IEEE 754:1985 float
   vr["FD"] = "Floating Point Double"; // 64-bit IEEE 754:1985 double
   vr["IS"] = "Integer String";        // At most 12 bytes
   vr["LO"] = "Long String";           // At most 64 chars
   vr["LT"] = "Long Text";             // At most 10240 chars
   vr["OB"] = "Other Byte String";     // String of bytes (vr independant)
   vr["OW"] = "Other Word String";     // String of 16-bit words (vr dep)
   vr["PN"] = "Person Name";           // At most 64 chars
   vr["SH"] = "Short String";          // At most 16 chars
   vr["SL"] = "Signed Long";           // Exactly 4 bytes
   vr["SQ"] = "Sequence of Items";     // Not Applicable
   vr["SS"] = "Signed Short";          // Exactly 2 bytes
   vr["ST"] = "Short Text";            // At most 1024 chars
   vr["TM"] = "Time";                  // At most 16 bytes
   vr["UI"] = "Unique Identifier";     // At most 64 bytes
   vr["UL"] = "Unsigned Long ";        // Exactly 4 bytes
   vr["UN"] = "Unknown";               // Any length of bytes
   vr["US"] = "Unsigned Short ";       // Exactly 2 bytes
   vr["UT"] = "Unlimited Text";        // At most 2^32 -1 chars
}

gdcmVR::~gdcmVR() {
   vr.clear();
}

int gdcmVR::Count(VRKey key) {
   return vr.count(key);
}
