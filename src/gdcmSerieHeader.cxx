/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmSerieHeader.cxx,v $
  Language:  C++
  Date:      $Date: 2005/01/31 12:19:34 $
  Version:   $Revision: 1.17 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmSerieHeader.h"
#include "gdcmDirList.h"
#include "gdcmFile.h"
#include "gdcmDebug.h"

#include <math.h>
#include <algorithm>
#include <vector>

namespace gdcm 
{
typedef std::list<File* > GdcmFileList;
typedef std::vector<File* > GdcmFileVector;

//-----------------------------------------------------------------------------
// Constructor / Destructor
/**
 * \brief   Constructor from a given SerieHeader
 */
SerieHeader::SerieHeader()
{
   //TODO 
   //CoherentGdcmFileList.clear();
}

/**
 * \brief   Canonical destructor.
 */
SerieHeader::~SerieHeader()
{
   // For all the Coherent File lists of the gdcm::Serie
   GdcmFileList *l = GetFirstCoherentFileList();
   while (l)
   { 
      // For all the files of a Coherent File list
      for (GdcmFileList::iterator it = l->begin();
                                  it != l->end(); 
                                ++it)
      {
         delete *it;
      }
      l->clear();
      l = GetNextCoherentFileList();
   }
}

//-----------------------------------------------------------------------------
// Print

/**
 * \brief   Canonical printer.
 */
void SerieHeader::Print()
{
   // For all the Coherent File lists of the gdcm::Serie
   CoherentFileListmap::iterator itl = CoherentGdcmFileListHT.begin();
   if ( itl == CoherentGdcmFileListHT.end() )
   {
      gdcmVerboseMacro( "No Coherent File list found" );
      return;
   }
   while (itl != CoherentGdcmFileListHT.end())
   { 
      std::cout << "Serie UID :[" << itl->first << "]" << std::endl;

      // For all the files of a Coherent File list
      for (GdcmFileList::iterator it =  (itl->second)->begin();
                                  it != (itl->second)->end(); 
                                ++it)
      {
         std::cout << " --- " << (*it)->GetFileName() << std::endl;
      }
      ++itl;
   }
}

//-----------------------------------------------------------------------------
// Public
/**
 * \brief add a gdcm::File to the list corresponding to its Serie UID
 * @param   filename Name of the file to deal with
 */
void SerieHeader::AddFileName(std::string const &filename)
{
   //directly use string and not const char*:
   File *header = new File( filename ); 
   if( header->IsReadable() )
   {
      // 0020 000e UI REL Series Instance UID
      std::string uid =  header->GetEntryValue (0x0020, 0x000e);
      // if uid == GDCM_UNFOUND then consistently we should find GDCM_UNFOUND
      // no need here to do anything special

      if ( CoherentGdcmFileListHT.count(uid) == 0 )
      {
         gdcmVerboseMacro(" New Serie UID :[" << uid << "]");
         // create a std::list in 'uid' position
         CoherentGdcmFileListHT[uid] = new GdcmFileList;
      }
      // Current Serie UID and DICOM header seems to match add the file:
      CoherentGdcmFileListHT[uid]->push_back( header );
   }
   else
   {
      gdcmVerboseMacro("Could not read file: " << filename );
      delete header;
   }
}

/**
 * \brief Sets the root Directory
 * @param   dir Name of the directory to deal with
 * @param recursive whether we want explore recursively the Directory
 */
void SerieHeader::SetDirectory(std::string const &dir, bool recursive)
{
   DirList dirList(dir, recursive); // OS specific
  
   DirListType filenames_list = dirList.GetFilenames();
   for( DirListType::const_iterator it = filenames_list.begin(); 
        it != filenames_list.end(); ++it)
   {
      AddFileName( *it );
   }
}

/**
 * \brief Sorts the given File List
 * \warning This could be implemented in a 'Strategy Pattern' approach
 *          But as I don't know how to do it, I leave it this way
 *          BTW, this is also a Strategy, I don't know this is the best approach :)
 */
void SerieHeader::OrderGdcmFileList(GdcmFileList *CoherentGdcmFileList)
{
   if( ImagePositionPatientOrdering( CoherentGdcmFileList ) )
   {
      return ;
   }
   else if( ImageNumberOrdering(CoherentGdcmFileList ) )
   {
      return ;
   }
   else  
   {
      FileNameOrdering(CoherentGdcmFileList );
   }
}

/**
 * \brief   Get the first List while visiting the CoherentFileListHT
 * @return  The first GdcmFileList if found, otherwhise NULL
 */
 std::list<File* > *SerieHeader::GetFirstCoherentFileList()
// Why doesn't it compile ?!?
//GdcmFileList *SerieHeader::GetFirstCoherentFileList()
{
   ItListHt = CoherentGdcmFileListHT.begin();
   if( ItListHt != CoherentGdcmFileListHT.end() )
      return ItListHt->second;
   return NULL;
}

/**
 * \brief   Get the next List while visiting the CoherentFileListHT
 * \note : meaningfull only if GetFirstCoherentFileList already called
 * @return  The next GdcmFileList if found, otherwhise NULL
 */
std::list<File* > *SerieHeader::GetNextCoherentFileList()
{
   gdcmAssertMacro (ItListHt != CoherentGdcmFileListHT.end());
  
   ++ItListHt;
   if ( ItListHt != CoherentGdcmFileListHT.end() )
      return ItListHt->second;
   return NULL;
}

/**
 * \brief   Get the Coherent Files list according to its Serie UID
 * @param SerieUID SerieUID
 * \return  pointer to the Coherent Filseslist if found, otherwhise NULL
 */
GdcmFileList *SerieHeader::GetCoherentFileList(std::string SerieUID)
{
   if ( CoherentGdcmFileListHT.count(SerieUID) == 0 )
      return 0;     
   return CoherentGdcmFileListHT[SerieUID];
}


//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
/**
 * \brief sorts the images, according to their Patient Position
 *  We may order, considering :
 *   -# Image Position Patient
 *   -# Image Number
 *   -# More to come :-)
 * @param CoherentGdcmFileList Coherent File list (same Serie UID) to sort
 * @return false only if the header is bugged !
 */
bool SerieHeader::ImagePositionPatientOrdering( 
                                       GdcmFileList *CoherentGdcmFileList )
//based on Jolinda's algorithm
{
   //iop is calculated based on the file file
   float cosines[6];
   float normal[3];
   float ipp[3];
   float dist;
   float min = 0, max = 0;
   bool first = true;
   int n=0;
   std::vector<float> distlist;

   //!\todo rewrite this for loop.
   for ( GdcmFileList::const_iterator 
         it = CoherentGdcmFileList->begin();
         it != CoherentGdcmFileList->end(); ++it )
   {
      if( first ) 
      {
         (*it)->GetImageOrientationPatient( cosines );
      
         // You only have to do this once for all slices in the volume. Next, 
         // for each slice, calculate the distance along the slice normal 
         // using the IPP tag ("dist" is initialized to zero before reading 
         // the first slice) :
         normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
         normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
         normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];
  
         ipp[0] = (*it)->GetXOrigin();
         ipp[1] = (*it)->GetYOrigin();
         ipp[2] = (*it)->GetZOrigin();

         dist = 0;
         for ( int i = 0; i < 3; ++i )
         {
            dist += normal[i]*ipp[i];
         }
    
         if( dist == 0 )
         {
            return false;
         }

         distlist.push_back( dist );

         max = min = dist;
         first = false;
      }
      else 
      {
         ipp[0] = (*it)->GetXOrigin();
         ipp[1] = (*it)->GetYOrigin();
         ipp[2] = (*it)->GetZOrigin();
  
         dist = 0;
         for ( int i = 0; i < 3; ++i )
         {
            dist += normal[i]*ipp[i];
         }

         if( dist == 0 )
         {
            return false;
         }
      
         distlist.push_back( dist );

         min = (min < dist) ? min : dist;
         max = (max > dist) ? max : dist;
      }
      ++n;
   }

   // Then I order the slices according to the value "dist". Finally, once
   // I've read in all the slices, I calculate the z-spacing as the difference
   // between the "dist" values for the first two slices.
   GdcmFileVector CoherentGdcmFileVector(n);
   // CoherentGdcmFileVector.reserve( n );
   CoherentGdcmFileVector.resize( n );
   // gdcmAssertMacro( CoherentGdcmFileVector.capacity() >= n );

   float step = (max - min)/(n - 1);
   int pos;
   n = 0;
    
   //VC++ don't understand what scope is !! it -> it2
   for (GdcmFileList::const_iterator it2  = CoherentGdcmFileList->begin();
        it2 != CoherentGdcmFileList->end(); ++it2, ++n)
   {
      //2*n sort algo !!
      //Assumption: all files are present (no one missing)
      pos = (int)( fabs( (distlist[n]-min)/step) + .5 );

      // a Dicom 'Serie' may contain scout views
      // and images may have differents directions
      // -> More than one may have the same 'pos'
      // Sorting has then NO meaning !
      if (CoherentGdcmFileVector[pos]==NULL)
         CoherentGdcmFileVector[pos] = *it2;
      else
         return false;
   }

   CoherentGdcmFileList->clear();  // doesn't delete list elements, only node
  
   //VC++ don't understand what scope is !! it -> it3
   for (GdcmFileVector::const_iterator it3  = CoherentGdcmFileVector.begin();
        it3 != CoherentGdcmFileVector.end(); ++it3)
   {
      CoherentGdcmFileList->push_back( *it3 );
   }

   distlist.clear();
   CoherentGdcmFileVector.clear();

   return true;
}

/**
 * \brief sorts the images, according to their Image Number
 * @param CoherentGdcmFileList Coherent File list (same Serie UID) to sort 
 * @return false only if the header is bugged !
 */
bool SerieHeader::ImageNumberOrdering(GdcmFileList *CoherentGdcmFileList) 
{
   int min, max, pos;
   int n = 0;//CoherentGdcmFileList.size() is a O(N) operation
   unsigned char *partition;
  
   GdcmFileList::const_iterator it = CoherentGdcmFileList->begin();
   min = max = (*it)->GetImageNumber();

   for (; it != CoherentGdcmFileList->end(); ++it, ++n)
   {
      pos = (*it)->GetImageNumber();

      //else
      min = (min < pos) ? min : pos;
      max = (max > pos) ? max : pos;
   }

   // Find out if sorting worked:
   if( min == max || max == 0 || max > (n+min)) return false;

   //bzeros(partition, n); //This function is deprecated, better use memset.
   partition = new unsigned char[n];
   memset(partition, 0, n);

   GdcmFileVector CoherentGdcmFileVector(n);

   //VC++ don't understand what scope is !! it -> it2
   for (GdcmFileList::const_iterator it2 = CoherentGdcmFileList->begin();
        it2 != CoherentGdcmFileList->end(); ++it2)
   {
      pos = (*it2)->GetImageNumber();
      CoherentGdcmFileVector[pos - min] = *it2;
      partition[pos - min]++;
   }
  
   unsigned char mult = 1;
   for( int i=0; i<n ; i++ )
   {
      mult *= partition[i];
   }

   //VC++ don't understand what scope is !! it -> it3
   CoherentGdcmFileList->clear();  // doesn't delete list elements, only node
   for ( GdcmFileVector::const_iterator it3 = CoherentGdcmFileVector.begin();
         it3 != CoherentGdcmFileVector.end(); ++it3 )
   {
      CoherentGdcmFileList->push_back( *it3 );
   }
   CoherentGdcmFileVector.clear();
  
   delete[] partition;

   return mult != 0;
}

/**
 * \brief sorts the images, according to their File Name
 * @param CoherentGdcmFileList Coherent File list (same Serie UID) to sort
 * @return false only if the header is bugged !
 */
bool SerieHeader::FileNameOrdering(GdcmFileList *)
{
   //using the sort
   //sort(CoherentGdcmFileList.begin(), CoherentGdcmFileList.end());
   return true;
}

} // end namespace gdcm
//-----------------------------------------------------------------------------
