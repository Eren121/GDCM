/*=========================================================================
                                                                                
  Program:   gdcm
  Module:    $RCSfile: gdcmHeaderHelper.cxx,v $
  Language:  C++
  Date:      $Date: 2004/06/21 04:18:26 $
  Version:   $Revision: 1.38 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                                
=========================================================================*/

#include "gdcmHeaderHelper.h"
#include "gdcmDirList.h"

#include "gdcmDebug.h"
#include <math.h>
#include <algorithm>
#include <vector>

//-----------------------------------------------------------------------------
// Constructor / Destructor
gdcmSerieHeader::~gdcmSerieHeader(){
  /// \todo
  for (std::list<gdcmHeader*>::iterator it  = CoherentGdcmFileList.begin();
        it != CoherentGdcmFileList.end(); it++)
  {
    delete *it;
  }
  CoherentGdcmFileList.clear();
}

//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
// Public
/**
 * \brief add a gdcmFile to the list based on file name
 * @param   filename Name of the file to deal with
 */
void gdcmSerieHeader::AddFileName(std::string filename) {
  gdcmHeader *GdcmFile = new gdcmHeader( filename );
  this->CoherentGdcmFileList.push_back( GdcmFile );
}

/**
 * \brief add a gdcmFile to the list
 * @param   file gdcmHeader to add
 */
void gdcmSerieHeader::AddGdcmFile(gdcmHeader *file){
  this->CoherentGdcmFileList.push_back( file );
}

/**
 * \brief Sets the Directory
 * @param   dir Name of the directory to deal with
 */
void gdcmSerieHeader::SetDirectory(std::string dir){
  gdcmDirList filenames_list(dir);  //OS specific
  
  for(gdcmDirList::iterator it = filenames_list.begin(); 
      it !=filenames_list.end(); it++)
  {
    gdcmHeader *file = new gdcmHeader( it->c_str() );
    this->CoherentGdcmFileList.push_back( file );
  }
}

/**
 * \brief Sorts the File List
 * \warning This could be implemented in a 'Strategy Pattern' approach
 *          But as I don't know how to do it, I leave it this way
 *          BTW, this is also a Strategy, I don't know this is the best approach :)
*/
void gdcmSerieHeader::OrderGdcmFileList(){
  if( ImagePositionPatientOrdering() ) {
    return ;
  }
  else if( ImageNumberOrdering() ) {
    return ;
  } else  {
    FileNameOrdering();
  }
}

/**
 * \brief Gets the *coherent* File List
 * @return the *coherent* File List
*/
std::list<gdcmHeader*> &gdcmSerieHeader::GetGdcmFileList() {
  return CoherentGdcmFileList;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private
/**
 * \ingroup gdcmHeader
 * \brief sorts the images, according to their Patient Position
 *  We may order, considering :
 *   -# Image Number
 *   -# Image Position Patient
 *   -# More to come :)
 * @return false only if the header is bugged !
 */
bool gdcmSerieHeader::ImagePositionPatientOrdering()
//based on Jolinda's algorithm
{
  //iop is calculated based on the file file
  float *cosines = new float[6];
  float normal[3];
  float ipp[3];
  float dist;
  float min, max;
  bool first = true;
  int n=0;
  std::vector<float> distlist;

  //!\todo rewrite this for loop.
  for (std::list<gdcmHeader*>::iterator it  = CoherentGdcmFileList.begin();
        it != CoherentGdcmFileList.end(); it++)
  {
    if(first) {
      (*it)->GetImageOrientationPatient(cosines);
      
      //You only have to do this once for all slices in the volume. Next, for
      //each slice, calculate the distance along the slice normal using the IPP
      //tag ("dist" is initialized to zero before reading the first slice) :
      normal[0] = cosines[1]*cosines[5] - cosines[2]*cosines[4];
      normal[1] = cosines[2]*cosines[3] - cosines[0]*cosines[5];
      normal[2] = cosines[0]*cosines[4] - cosines[1]*cosines[3];
  
      ipp[0] = (*it)->GetXOrigin();
      ipp[1] = (*it)->GetYOrigin();
      ipp[2] = (*it)->GetZOrigin();

      dist = 0;
      for (int i = 0; i < 3; ++i)
          dist += normal[i]*ipp[i];
    
      if( dist == 0 )
      {
        delete[] cosines;
        return false;
      }

      distlist.push_back( dist );

      max = min = dist;
      first = false;
    }
    else {
      ipp[0] = (*it)->GetXOrigin();
      ipp[1] = (*it)->GetYOrigin();
      ipp[2] = (*it)->GetZOrigin();
  
      dist = 0;
      for (int i = 0; i < 3; ++i)
          dist += normal[i]*ipp[i];

      if( dist == 0 )
      {
        delete[] cosines;
        return false;
      }
      
      distlist.push_back( dist );

      min = (min < dist) ? min : dist;
      max = (max > dist) ? max : dist;
    }
    n++;
  }

    //Then I order the slices according to the value "dist". Finally, once
    //I've read in all the slices, I calculate the z-spacing as the difference
    //between the "dist" values for the first two slices.
    std::vector<gdcmHeader*> CoherentGdcmFileVector(n);
    //CoherentGdcmFileVector.reserve( n );
    CoherentGdcmFileVector.resize( n );
    //assert( CoherentGdcmFileVector.capacity() >= n );

    float step = (max - min)/(n - 1);
    int pos;
    n = 0;
    
    //VC++ don't understand what scope is !! it -> it2
    for (std::list<gdcmHeader*>::iterator it2  = CoherentGdcmFileList.begin();
        it2 != CoherentGdcmFileList.end(); it2++, n++)
    {
      //2*n sort algo !!
      //Assumption: all files are present (no one missing)
      pos = (int)( fabs( (distlist[n]-min)/step) + .5 );
            
      CoherentGdcmFileVector[pos] = *it2;
    }

  CoherentGdcmFileList.clear();  //this doesn't delete list's element, node only
  
  //VC++ don't understand what scope is !! it -> it3
  for (std::vector<gdcmHeader*>::iterator it3  = CoherentGdcmFileVector.begin();
        it3 != CoherentGdcmFileVector.end(); it3++)
  {
    CoherentGdcmFileList.push_back( *it3 );
  }

  distlist.clear();
  CoherentGdcmFileVector.clear();
  delete[] cosines;
  
  return true;
}

/**
 * \ingroup gdcmHeader
 * \brief sorts the images, according to their Image Number
 * @return false only if the header is bugged !
 */

bool gdcmSerieHeader::ImageNumberOrdering() {
  int min, max, pos;
  int n = 0;//CoherentGdcmFileList.size() is a O(N) operation !!
  unsigned char *partition;
  
  std::list<gdcmHeader*>::iterator it  = CoherentGdcmFileList.begin();
  min = max = (*it)->GetImageNumber();

  for (; it != CoherentGdcmFileList.end(); it++, n++)
  {
    pos = (*it)->GetImageNumber();

    //else
    min = (min < pos) ? min : pos;
  }

  //bzeros(partition, n); //Cette fonction est déconseillée, utilisez plutôt memset.
  partition = new unsigned char[n];
  memset(partition, 0, n);

  std::vector<gdcmHeader*> CoherentGdcmFileVector(n);

  //VC++ don't understand what scope is !! it -> it2
  for (std::list<gdcmHeader*>::iterator it2  = CoherentGdcmFileList.begin();
        it2 != CoherentGdcmFileList.end(); it2++)
  {
    pos = (*it2)->GetImageNumber();
    CoherentGdcmFileVector[pos - min] = *it2;
    partition[pos - min]++;
  }
  
  unsigned char mult = 1;
  for(int i=0; i<n ; i++)
  {
    mult *= partition[i];
  }

  //VC++ don't understand what scope is !! it -> it3
  CoherentGdcmFileList.clear();  //this doesn't delete list's element, node only
  for (std::vector<gdcmHeader*>::iterator it3  = CoherentGdcmFileVector.begin();
        it3 != CoherentGdcmFileVector.end(); it3++)
  {
    CoherentGdcmFileList.push_back( *it3 );
  }
  CoherentGdcmFileVector.clear();
  
  delete[] partition;
  return (mult!=0);
}


/**
 * \ingroup gdcmHeader
 * \brief sorts the images, according to their File Name
 * @return false only if the header is bugged !
 */
 bool gdcmSerieHeader::FileNameOrdering() {
  //using the sort
  //sort(CoherentGdcmFileList.begin(), CoherentGdcmFileList.end());
  return true;
}

//-----------------------------------------------------------------------------
