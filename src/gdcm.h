// gdcm.h

#ifndef GDCM_H
#define GDCM_H

//using namespace std;

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

#include "gdcmException.h"
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
