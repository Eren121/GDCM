// gdcmDocEntry.cxx
//-----------------------------------------------------------------------------
//

#include "gdcmDocEntry.h"
#include "gdcmTS.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"

#include <iomanip> // for std::ios::left, ...

// CLEAN ME
#define MAX_SIZE_PRINT_ELEMENT_VALUE 64

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \ingroup gdcmDocEntry
 * \brief   Constructor from a given gdcmDictEntry
 * @param   in Pointer to existing dictionary entry
 */
gdcmDocEntry::gdcmDocEntry(gdcmDictEntry* in) {
   ImplicitVR = false;
   entry = in;
}

void gdcmDocEntry::Print(std::ostream & os) {
   std::ostringstream s;
   s << std::endl;
   PrintCommonPart(os);
   os << s.str();
}

//-----------------------------------------------------------------------------
// Print
/**
 * \ingroup gdcmDocEntry
 * \brief   Prints the common part of gdcmValEntry, gdcmBinEntry, gdcmSeqEntry
 */
void gdcmDocEntry::PrintCommonPart(std::ostream & os) {

   printLevel=2; // FIXME
   
   size_t o;
   unsigned short int g, e;
   char st[20];
   TSKey v;
   std::string d2, vr;
   std::ostringstream s;
   guint32 lgth;
   char greltag[10];  //group element tag

   g  = GetGroup();
   e  = GetElement();
   o  = GetOffset();
   vr = GetVR();
   sprintf(greltag,"%04x|%04x ",g,e);           
   s << greltag ;
       
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
   os << s.str();      
}

//-----------------------------------------------------------------------------
// Public

/**
 * \ingroup gdcmDocEntry
 * \brief   Gets the full length of the elementary DocEntry (not only value length)
 */
guint32 gdcmDocEntry::GetFullLength(void) {
   guint32 l;
   l = GetReadLength();
   if ( IsImplicitVR() ) 
      l = l + 8;  // 2 (gr) + 2 (el) + 4 (lgth) 
   else    
      if ( GetVR()=="OB" || GetVR()=="OW" || GetVR()=="SQ" )
         l = l + 12; // 2 (gr) + 2 (el) + 2 (vr) + 2 (unused) + 4 (lgth)
      else
         l = l + 8;  // 2 (gr) + 2 (el) + 2 (vr) + 2 (lgth)
   return(l);
}

/**
 * \ingroup gdcmDocEntry
 * \brief   Copies all the attributes from an other DocEntry 
 */

void gdcmDocEntry::Copy (gdcmDocEntry* e) {
   this->entry        = e->entry;
   this->UsableLength = e->UsableLength;
   this->ReadLength   = e->ReadLength;
   this->ImplicitVR   = e->ImplicitVR;
   this->Offset       = e->Offset;
   this->printLevel   = e->printLevel;
   // TODO : remove gdcmDocEntry SQDepth
}

bool gdcmDocEntry::isItemDelimitor() {
   if ( (GetGroup() == 0xfffe) && (GetElement() == 0xe00d) )
      return true;
   else
      return false;      
}

bool gdcmDocEntry::isSequenceDelimitor() {
   if (GetGroup() == 0xfffe && GetElement() == 0xe0dd)
      return true;
   else
      return false; 
}

//-----------------------------------------------------------------------------
// Protected


//-----------------------------------------------------------------------------
// Private

//-----------------------------------------------------------------------------
