// gdcmValEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmValEntry.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

// CLEAN ME
#define MAX_SIZE_PRINT_ELEMENT_VALUE 128

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmValEntry
 * \brief   Constructor from a given gdcmDictEntry
 * @param   e Pointer to existing dictionary entry
 */
gdcmValEntry::gdcmValEntry(gdcmDictEntry* e) : gdcmDocEntry(e) {
   voidArea = NULL; // will be in BinEntry ?
}

/**
 * \brief   Canonical destructor.
 */
gdcmValEntry::~gdcmValEntry (void) {
   if (!voidArea)  // will be in BinEntry
      free(voidArea);
}


//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmValEntry
 * \brief   canonical Printer
 */
void gdcmValEntry::Print(std::ostream & os) { 

   std::ostringstream s; 
   size_t o;
   unsigned short int g, e;
   char st[20];
   TSKey v;
   std::string d2, vr;
     
   PrintCommonPart(os); 

   g  = GetGroup();
   
   if (g == 0xfffe) {
      s << std::endl;
      os << s.str(); 
      return;  // just to avoid identing all the remaining code     
   }
   
   e  = GetElement();
   vr = GetVR();
   gdcmTS * ts = gdcmGlobal::GetTS();
    
   if (voidArea != NULL) { // should be moved in gdcmBinEntry Printer (when any)
       s << " [gdcm::Non String Data Loaded in Unsecure Area (" 
         << GetLength() << ") ]";
   } 
   
   else {
      v  = GetValue();  // not applicable for SQ ...     
      d2 = CreateCleanString(v);  // replace non printable characters by '.'            
      if( (GetLength()<=MAX_SIZE_PRINT_ELEMENT_VALUE) || 
          (printLevel>=3)  || 
          (d2.find("gdcm::NotLoaded.") < d2.length()) )
         s << " [" << d2 << "]";
      else 
         s << " [gdcm::too long for print (" << GetLength() << ") ]";
   }
   
   // Display the UID value (instead of displaying only the rough code)  
   if (g == 0x0002) {  // Any more to be displayed ?
      if ( (e == 0x0010) || (e == 0x0002) )
         s << "  ==>\t[" << ts->GetValue(v) << "]";
   } else {
      if (g == 0x0008) {
         if ( (e == 0x0016) || (e == 0x1150)  )
            s << "  ==>\t[" << ts->GetValue(v) << "]";
      } else {
         if (g == 0x0004) {
	    if ( (e == 0x1510) || (e == 0x1512)  )
	       s << "  ==>\t[" << ts->GetValue(v) << "]";
	 }     
      }
   }
   //if (e == 0x0000) {        // elem 0x0000 --> group length 
   if ( (vr == "UL") || (vr == "US") || (vr == "SL") || (vr == "SS") ) {
      if (v == "4294967295") // to avoid troubles in convertion 
         sprintf (st," x(ffffffff)");
      else {
         if ( GetLength() !=0 )        
            sprintf(st," x(%x)", atoi(v.c_str()));//FIXME
	 else
	  sprintf(st," "); 
      }
      s << st;
   }
        
   s << std::endl;
   os << s.str();   
}
 

//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
