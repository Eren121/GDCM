// gdcmHeaderEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmHeaderEntry.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#ifdef GDCM_NO_ANSI_STRING_STREAM
#  include <strstream>
#  define  ostringstream ostrstream
# else
#  include <sstream>
#endif
#include <iomanip> // for std::ios::left, ...


#define MAX_SIZE_PRINT_ELEMENT_VALUE 64

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmHeaderEntry
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */
gdcmHeaderEntry::gdcmHeaderEntry(gdcmDictEntry* in) {
   ImplicitVR = false;
   voidArea = NULL; // unsecure memory area to hold 'non string' values
   entry = in;
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmHeaderEntry
 * \brief   canonical Printer
 */
void gdcmHeaderEntry::Print(std::ostream & os) {
   size_t o;
   unsigned short int g, e;
   char st[20];
   TSKey v;
   std::string d2, vr;
   gdcmTS * ts = gdcmGlobal::GetTS();
   std::ostringstream s;
   guint32 lgth;
   char greltag[10];  //group element tag

   g  = GetGroup();
   e  = GetElement();
   v  = GetValue();
   o  = GetOffset();
   vr = GetVR();
   sprintf(greltag,"%04x|%04x ",g,e);           
   s << greltag ;
       
   d2 = CreateCleanString(v);  // replace non printable characters by '.'
   if (printLevel>=2) { 
      s << "lg : ";
      lgth = GetReadLength(); // ReadLength, as opposed to UsableLength
      if (lgth == 0xffffffff) {
         sprintf(st,"x(ffff)");  // I said : "x(ffff)" !
         s.setf(std::ios::left);
         s << std::setw(10-strlen(st)) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << "-1";      
      } else {
         sprintf(st,"x(%x)",lgth);
         s.setf(std::ios::left);
         s << std::setw(10-strlen(st)) << " ";  
         s << st << " ";
         s.setf(std::ios::left);
         s << std::setw(8) << lgth; 
      }
      s << " Off.: ";
      sprintf(st,"x(%x)",o); 
      s << std::setw(10-strlen(st)) << " ";
      s << st << " ";
      s << std::setw(8) << o; 
   }

   s << "[" << vr  << "] ";

   if (printLevel>=1) {      
      s.setf(std::ios::left);
      s << std::setw(66-GetName().length()) << " ";
   }
    
   s << "[" << GetName()<< "]";

   if (voidArea != NULL) {
       s << " [gdcm::Non String Data Loaded in Unsecure Area (" 
         << GetLength() << ") ]";
   } 
   
   else {             
      if( (GetLength()<MAX_SIZE_PRINT_ELEMENT_VALUE) || 
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

/**
 * \ingroup gdcmHeaderEntry
 * \brief   Gets the full length of the HeaderEntry (not only value length)
 */
guint32 gdcmHeaderEntry::GetFullLength(void) {
   guint32 l;
   l = GetLength();
   if ( IsImplicitVR() ) 
      l = l + 8;  // 2 (gr) + 2 (el) + 4 (lgth) 
   else    
      if ( GetVR()=="OB" || GetVR()=="OW" || GetVR()=="SQ" )
         l = l + 12; // 2 (gr) + 2 (el) + 2 (vr) + 2 (unused) + 4 (lgth)
      else
         l = l + 8;  // 2 (gr) + 2 (el) + 2 (vr) + 2 (lgth)
   return(l);
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
