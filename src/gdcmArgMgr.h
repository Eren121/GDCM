/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmArgMgr.h,v $
  Language:  C++
  Date:      $Date: 2005/06/07 07:46:50 $
  Version:   $Revision: 1.2 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#ifndef  ___ARG_MGR__
#define  ___ARG_MGR__

#include "gdcmCommon.h"
#include <stdlib.h> // For atof

namespace gdcm
{

#define ID_RFILE_TEXT   "rt"
#define ARGMAXCOUNT 100   // Maximum number of arguments
#define ARG_LONG_MAX          1000

// default file name
#define ARG_DEFAULT_PARAMOUT    "fileout.par"
#define ARG_DEFAULT_LOGFILE     "gdcm.log"

#define  ARG_LABEL_LOGFILE       "LOG"
#define  ARG_LABEL_PARAMOUT      "paramout"

#define START_USAGE(usage)   char *usage[] = {
#define FINISH_USAGE         0};

//-----------------------------------------------------------------------------
/**
 * \brief   class designed for command line arguments management
 *          (to make programmer's live easier) 
 *          NOT Dicom dependant (could be used for any kind 
 *                              of 'command line program')        
 */
 
class GDCM_EXPORT ArgMgr
{
public:
   ArgMgr(int argc, char **argv);
   ~ArgMgr();

   int    ArgMgrDefined           (char *);      /* Checks if Param is defined*/
   char  *ArgMgrValue             (char *);      /* Returns Param value       */
   char  *ArgMgrUnused            (void);        /* Returns a never used arg. */
   int    ArgMgrSave              (char *);      /* Save of parameters out    */
   int    ArgMgrUsage             (char **);     /* Display program usage     */
   int    ArgMgrPrintUnusedLabels (void);        /* Prints unused labels      */

   int    ArgMgrGetInt   (char*, int);  /* Gets an int   (with default value)  */
   float  ArgMgrGetFloat (char*, float);/* Gets a float  (with default value)  */
   char  *ArgMgrGetString(char*, char*);/* Gets a string (with default value)  */
   int    ArgMgrGetLabel (char *, char *, int);
  
   int   ArgMgrWantInt   (char*, char**);  /* Demands an int                   */
   float ArgMgrWantFloat (char*, char**);  /* Demands a float                  */
   char *ArgMgrWantString(char*, char**);  /* Demands a string                 */
   int   ArgMgrWantLabel (char*, char *, char **);

   int   *ArgMgrGetListOfInt   (char *, int *);   /* Gets a list of int        */
   float *ArgMgrGetListOfFloat (char *, int *);   /* Gets a list of float      */
   char **ArgMgrGetListOfString(char *, int *);   /* Gets a list of string     */

   int   * ArgMgrGetIntEnum     (char *, int *); /* Gets a list of int intervals  */
   float * ArgMgrGetFloatEnum   (char *, int *); /* Gets a list of float intervals*/

private :

   int    FiltreLong      (char *);
   char  *LoadedParam     (char *, FILE *);
   int    ArgLoadFromFile (char *);
   void   ArgStdArgs      (void);

   // These ones are 'general purpose methods'
   char  *maj             (char *);
   char  *Majuscule       (char *);
   int IdStrCountChar     (char *chaine, int caract);
   int *IdStrIntEnum      (char* value, int *number);
   float *IdStrFloatEnum  (char* value, int *number);

// --------------- Attributes ------------------------------

private :

   char *ArgParamOut;          /* Output File Name for param */

   char *ArgUsed;              /* Used Arguments             */
   char *ArgLab[ARGMAXCOUNT];  /* Arguments Labels           */
   char *ArgStr[ARGMAXCOUNT];  /* Arguments 'strings'        */
   int   ArgCount;             /* Number of arguments passed */
   char *Appel;                
};
} // end namespace gdcm

#endif