// gdcm.h

// gdcmlib Intro:  
// * gdcmlib is a library dedicated to reading and writing dicom files.
// * LGPL for the license
// * lightweigth as opposed to CTN or DCMTK which come bundled which try
//   to implement the full DICOM standard (networking...). gdcmlib concentrates
//   on reading and writing
// * Formats: this lib should be able to read ACR-NEMA v1 and v2, Dicom v3 (as
//   stated in part10). [cf dcmtk/dcmdata/docs/datadict.txt]
// * Targeted plateforms: Un*xes and Win32/VC++6.0
//
//
// TODO
// The declarations commented out and starting with "TODO Swig" needed
// to be temporarily removed for swig to proceed correctly (in fact
// problems appears at loading of _gdcm.[so/dll]). So, simply uncomment
// the declaration once you provided the definition of the method...

#ifndef GDCM_H
#define GDCM_H

using namespace std;

#include <iostream>
#include <stddef.h>   // For size_t
#include <stdio.h>    // FIXME For FILE on GCC only
#include <list>
#include <map>
#include "gdcmException.h"


// The requirement for the hash table (or map) that we shall use:
// 1/ First, next, last (iterators)
// 2/ should be sortable (i.e. sorted by TagKey). This
//    condition shall be droped since the Win32/VC++
//    implementation doesn't look a sorted one. Pffff....
// 3/ Make sure we can setup some default size value,
//    which should be around 4500 entries which is the
//    average dictionary size (said JPR)
//
// JPR CLEAN
// En fait, je disais que dans LE Directory Dicom (dans son etat 2001)
// il y a +/- 1600 entrees.
// Une valeur raisonable pour un  majorant du nombre d'entrees
// dans une entete DICOM d'une image semble semble etre 300
// Si on 'decortique' les elements SQ (ce qui ne semble pas etre fait pour le moment)
// on risque en fait de depasser ... un nombre non previsible dans le cas d'une entree SQ
// contenant lui même un tres grand nombre d'entrees ?!?)
// Quant au nombre d'entrees dans un DICOMDIR, c'est encore pire : il n'est limité
// que par la taille d'un CD-ROM (les DVD-ROM ne sont pas encore pris en compte)
// On peut s'attendre a 30 entrees par fichier dicom présent sur le CD-ROM
// REMARQUE : il faudra se pencher sur le pb de la creation du DICOMDIR lorsqu'on voudra 
// exporter des images lisibles par les consoles cliniques 
// et pas seulement importables dans e-film. 

////////////////////////////////////////////////////////////////////////////
// Tag based hash tables.
// We shall use as keys the strings (as the C++ type) obtained by
// concatenating the group value and the element value (both of type
// unsigned 16 bit integers in Dicom) expressed in hexadecimal.
// Example: consider the tag given as (group, element) = (0x0010, 0x0010).
// Then the corresponding TagKey shall be the string 0010|0010 (where
// the | (pipe symbol) acts as a separator). Refer to 
// gdcmDictEntry::TranslateToKey for this conversion function.

#include "gdcmCommon.h"
#include "gdcmDictEntry.h"
#include "gdcmDict.h"
#include "gdcmDictSet.h"
#include "gdcmElValue.h"
#include "gdcmElValSet.h"
#include "gdcmHeader.h"
#include "gdcmFile.h"

//class gdcmSerie : gdcmFile;
//class gdcmMultiFrame : gdcmFile;

#endif // #ifndef GDCM_H
