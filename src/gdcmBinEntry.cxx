// gdcmBinEntry.cxx
//-----------------------------------------------------------------------------
//
#include "gdcmBinEntry.h"


//-----------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   Constructor from a given gdcmBinEntry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDictEntry* e) : gdcmValEntry(e) {

}

/**
 * \brief   Constructor from a given gdcmBinEntry
 * @param   e Pointer to existing Doc entry
 */
gdcmBinEntry::gdcmBinEntry(gdcmDocEntry* e) : gdcmValEntry(e->GetDictEntry()){
   this->UsableLength = e->GetLength();
	this->ReadLength   = e->GetReadLength();	
	this->ImplicitVR   = e->IsImplicitVR();
	this->Offset       = e->GetOffset();	
	this->printLevel   = e->GetPrintLevel();	
	this->SQDepthLevel = e->GetDepthLevel();	
	
   this->voidArea = NULL; // let's be carefull !
}

/**
 * \brief   Canonical destructor.
 */
gdcmBinEntry::~gdcmBinEntry(){
   free (voidArea);
}


//-----------------------------------------------------------------------------
// Print
/*
 * \ingroup gdcmDocEntry
 * \brief   canonical Printer
 */
 
void gdcmBinEntry::Print(std::ostream &os) {
   PrintCommonPart(os);
   std::cout << " gdcmBinEntry : Print, so WHAT ?" <<std::endl;
}
//-----------------------------------------------------------------------------
// Public

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
   
//-----------------------------------------------------------------------------
