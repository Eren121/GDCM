/*=========================================================================
  
  Program:   gdcm
  Module:    $RCSfile: gdcmArgMgr.cxx,v $
  Language:  C++
  Date:      $Date: 2005/06/06 12:37:58 $
  Version:   $Revision: 1.1 $
  
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
  
=========================================================================*/

#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <string.h>  // For strlen

#include <string.h>  // For strtok and strlen
#include <stdlib.h>  // For strtol and strtod

#include "gdcmArgMgr.h"

#define ARG_DEFAULT_PARAMOUT    "fileout.par"
#define ARG_DEFAULT_LOGFILE     "gdcm.log"


namespace gdcm 
{
//-------------------------------------------------------------------------
// Constructor / Destructor

/**
 * \brief   constructor
 * @param argc arguments count, as passed to main()
 * @param argv  pointers array on the arguments passed to main()  
 */
 ArgMgr::ArgMgr(int argc, char *argv[])
 {
   int i;
   int nblettre;
   ArgUsed = NULL;
   Appel   = NULL;
  
   /* Read the parameters of the command line *************************/
   for ( ArgCount=0, nblettre=1 , i=0; i<argc; i++) 
   {
      if ( FiltreLong(argv[i]) ) 
      { 
          std::cout << "Argument too long ( > "
                    << ARG_LONG_MAX << ")" << std::endl;
          return;
      }
      if ( argv[i][0] == '@' )
      {                       
         nblettre  += ArgLoadFromFile ( &argv[i][1] );   
      }
      else
      {                                         
         ArgLab [ArgCount] = strcpy ( (char *)malloc(strlen(argv[i])+1), argv[i] ) ;
         nblettre  += 1 + strlen(ArgLab[ArgCount]);     
         ArgCount++;                               
      }
      if (ArgCount >= ARGMAXCOUNT )      
      {
          std::cout << "Too many Arguments ( more than "
                    << ARGMAXCOUNT << ")" << std::endl; 
          return;
      }
   }

   /* Fills an array with the alreadu used parameters ****/
   ArgUsed = (char *)calloc (1, ArgCount );

   /* Builds the full string with all the parameters  **************/
   Appel = (char *) calloc (1, nblettre );

   for ( *Appel = '\0', i=0; i<ArgCount; i++)
   {
      strcat ( Appel, ArgLab [i] ) ;
      strcat ( Appel, " " ) ;
   }

   /* Splitting label from label value *************************************/
   for ( i=0; i<ArgCount; i++) 
   {
      char * egaloufin = ArgLab[i] ;
      while ( (*egaloufin != '\0') && (*egaloufin != '=') ) 
         egaloufin ++ ;
      if ( *egaloufin ) *(egaloufin++) = '\0';
      ArgStr[i]= egaloufin;
   }

   /* Set labels to upper-case (labels are not case sensitive ) *********/
   for ( i=0; i<ArgCount; i++)
      ArgLab[i] = Majuscule ( ArgLab[i] ) ;

  /* Standard arguments are managed by ArgStdArgs **********************/
   ArgStdArgs(); 
 }

/**
 * \brief  canonical destructor
 */
ArgMgr::~ArgMgr()
{
   for(int i=0;i<ArgCount;i++)
      if(ArgLab[i])
         free(ArgLab[i]);
   if(ArgUsed)
      free(ArgUsed);
   if(Appel)
      free(Appel);
}
 
/**
 * \brief  checks if a parameter exists in the command line
 * @param searchParam  name of the searched parameter label
 * @return   true if parameter 'label' exists
 *           Actually, it returns 0 if label is not found
 *           else, returns the number of the spot it was found last time.
 */
int ArgMgr::ArgMgrDefined( char *searchParam )
{
  int i, trouve ;
  char *temp;
  temp = Majuscule ( searchParam ) ;
  for ( trouve = false, i = ArgCount-1; i>0; i-- )
  { 
    trouve = ! strcmp( ArgLab[i], temp ) ;
    if ( trouve )
    {
      ArgUsed[i] = true ;           
      for ( int j=1; j<i; j++)
      {                     
         if ( (!ArgUsed[j])&&(!strcmp(ArgLab[i],ArgLab[j])) )
            ArgUsed[j] = true ;
      }
      return i ;
    }
  }
  return false ;
}

/**
 * \brief  Gets the parameter value, read on the command line
 * @param param   name of the searched parameter label
 * @return   Value, as a characters string, of the parameter
 *            whose label is given.
 */
char *ArgMgr::ArgMgrValue ( char *param )
{
   int trouve ;
   if ( (trouve = ArgMgrDefined ( param )) != false )
      return ArgStr[trouve] ;
   else
      return NULL ;
}

/**
 * \brief  Search for the first not yet used label
 * @return Pointer to a char array holding the first non used label
 */
char *ArgMgr::ArgMgrUnused ( )
{
   int i ;
   for ( i=ArgCount-1; i>0; i-- )
   {
      if ( ! ArgUsed[i] )
      {
         ArgMgrDefined(ArgLab[i]);
         return ArgLab[i] ;
      }
  }
  return NULL ;
}

/**
 * \brief  Prints unused labels, if any
 * @return number of unused labels
 */
int ArgMgr::ArgMgrPrintUnusedLabels ()
{
   char *label;
   int i=0;
   while ( (label=ArgMgrUnused())!=0 )
   {
      if (i==0)
         std::cout << "\n Unused Labels:" << std::endl
                   << "=============="    << std::endl;
      std::cout << "Label : " << label << " = " 
                << ArgMgrValue(label) << std::endl;
      i++;
   }
   return i;
}

/**
 * \brief  Prints program usage
 * @param usage  array of pointers to the documentation lines
 *               of the program.
 * @return exception code
 */
int ArgMgr::ArgMgrUsage(char **usage_text )
{
   while ( *usage_text ) 
      std::cout << std::endl << *(usage_text++);
   std::cout << std::endl; 
   return (0);
}


/**
 * \brief Forget it, right now ... 
 * sauvegarde une chaine de caract. dans un fichier de parameters
 *         Le nom du fichier est celui specifie sur la ligne
 *         d'appel par : PARAMOUT=???
 *         ou, par defaut, celui donne par ARG_DEFAULT_PARAMOUT
 * @param param  Chaine de caractere definissant le parameter:
 * @return   Entier correspondant au rang dans la liste de labels
 */
int ArgMgr::ArgMgrSave ( char *param )
{
   static int   deja = 0;
   FILE         *fd;
   if ( *ArgParamOut == '\0' )
      return 0;
   if(deja) 
   {
      fd = fopen ( ArgParamOut, "a+" );
   }
   else
   {
      deja = 1;
      fd = fopen ( ArgParamOut, "w" );
   } 
   if ( !fd ) 
      return 0;
   fprintf ( fd, "%s\n", param );
   fclose  ( fd );
   return 1;
}

/**
 * \brief  Gets an int value passed as an argument to a program
 *         (use default value if not found)
 *  EXEMPLE:     int dimx = ArgMgrGetInt ( "DIMX", 256 );
 * @param label   parameter label
 * @param defaultVal default value
 * @return parameter value
 */
int ArgMgr::ArgMgrGetInt(char *label, int defaultVal)
{
   return ( (ArgMgrDefined(label))
            ? (atoi(ArgMgrValue(label)))
            : (defaultVal) );
}

/**
 * \brief  Gets a float value passed as an argument to a program
 *         (use default value if not found)
 *  EXEMPLE:     float scale = ArgMgrGetFloat ( "SCALE", 0.33 );
 * @param label   parameter label
 * @param defaultVal default value
 * @return parameter value
 */
float ArgMgr::ArgMgrGetFloat(char *label, float defaultVal)
{
   return     ( (ArgMgrDefined(label))
               ? (atof(ArgMgrValue(label)))
               : (defaultVal) );
}

/**
 * \brief  Gets a 'string' value passed as an argument to a program
 *         (use default value if not found)
 * @param label   parameter label
 * @param defaultVal default value
 * @return parameter value
 */
char *ArgMgr::ArgMgrGetString(char *label, char *defaultVal)
{
   return    ( (ArgMgrDefined(label)) 
              ? (ArgMgrValue(label))
              : (defaultVal) );
}

/**
 * \brief  Gets a value amongst a set od values
 *         (use default value if not found) 
 *         EXEMPLE:     int nlab = ArgMgrGetLabel("CONFIRM","NO\\YES", 0); 
 * @param label   parameter label
 * @param liste  character Chain describing the varous values.
 *               Value are separated by '\\'.
 *               Not case sensitive.
 * @param val  number of default value
 * @return   int : range of value amongst the values list
 */
int ArgMgr::ArgMgrGetLabel (char *label, char *liste, int val )
{
  char *lab;
  char *vallab;
  int i = 1;
  char *tmp;
  tmp = (char *) malloc(strlen(liste)+1);
  strcpy(tmp,liste);

  if ( (vallab = ArgMgrGetString(label,(char *)NULL)) != 0 ) 
  { 
     for ( lab = strtok (tmp,"\\"); 
           lab != 0; 
           lab = strtok(0L,"\\"), i++ )
     { 
        if ( strcmp(maj(lab),maj(vallab))==0)
           return i;
     } 
     val=0;
   }
   free(tmp);
   return val;
}

/**
 * \brief  Lecture d'un arg.de type label parmi un ensemble de labels
 *         EXEMPLE:     int nlab = ArgMgrWantLabel("CONFIRM","NO\\YES", usage); 
 * @param label   Parameter label
 * @param liste Chaine de caracteres decrivant les differents labels.
 *               Chaque label est separe par un '\\'.
 *               Aucune difference n'est faite entre maj./min.
 * @param usage Usage program (displayed if label not found)
 * @return   Entier correspondant au rang dans la liste de labels
 */
int ArgMgr::ArgMgrWantLabel (char *label, char *liste, char *usage[] )
{
   char *lab;
   char *vallab;
   int i = 1;
   if ( (vallab = ArgMgrGetString(label,0)) != 0 ) 
   {
      for ( lab = strtok (liste,"\\"); lab != 0; lab = strtok(0L,"\\"), i++ )
        if ( strcmp(maj(lab),maj(vallab))==0) 
           return i;
      return 0;
   }
   ArgMgrUsage(usage);
   return 0;
}

/**
 * \brief  Demands an int value passed as an argument to a program
 *         If not found usage is displayed and the prog aborted
 *  EXEMPLE:     int dimx = ArgMgrWantInt ( "DIMX", usage );
 * @param label   Parameter label
 * @param usage Usage program (displayed if label not found)
 * @return parameter value
 */
int ArgMgr::ArgMgrWantInt (char *label, char **usage)
{
   return        ( (ArgMgrDefined(label) ) 
                 ? (atoi(ArgMgrValue(label) ) ) 
                 : (ArgMgrUsage(usage),1) );
}

/**
 * \brief  Demands a float value passed as an argument to a program
 *         If not found usage is displayed and the prog aborted
 *  EXEMPLE:     float scale = ArgMgrWantFloat ( "SCALE", usage );
 * @param label   Parameter label
 * @param usage Usage program (displayed if label not found)
 * @return parameter value
 */
float ArgMgr::ArgMgrWantFloat (char *label, char **usage)
{
   return       ( (ArgMgrDefined(label) ) 
                ? (atof(ArgMgrValue(label) ) ) 
                : (ArgMgrUsage(usage),1.0) );
}

/**
 * \brief  Demands a 'string' value passed as an argument to a program
 *         If not found usage is displayed and the prog aborted
 *  EXEMPLE:     char *code = ArgMgrWantString ( "CODE", usage );
 * @param label   Parameter label
 * @param usage Usage program (displayed if label not found)
 * @return parameter value
 */
char *ArgMgr::ArgMgrWantString(char *label, char **usage)
{
   return      ( (ArgMgrDefined(label) ) 
               ? (ArgMgrValue(label) ) 
               : (ArgMgrUsage(usage),(char*)0) );
}

/**
 * \brief  decodage des elements d'un argument 'liste de STRING' de lgr qcq
 * @param label   pointer vers le label de la liste
 * @param number  taille de la liste  trouvee
 * @return   Pointer vers le tableau de lgr 'taille'
 *     Pointer NULL si erreur
 */
char **ArgMgr::ArgMgrGetListOfString ( char *label, int *nbElem )
{
  int taille;
  char  *value = ArgMgrValue(label);
  char **liste;
  char **elem;
  char  *chainecur; 
  if (!value)
     return 0;
  *nbElem = IdStrCountChar(value,',')+1; /* nb Elements = nb Commas +1 */
  taille = *nbElem;
  liste = (char **) malloc (sizeof(char*) * taille + strlen(value)+1);
  if ( !liste )
     return 0;
  value = strcpy( ((char*)liste)+sizeof(char*) * taille, value );
  for ( elem = liste, chainecur = strtok(value,", ");
        taille>0;
        taille--, chainecur = (chainecur) ? strtok ( 0, ", " ) : 0 )
  {
    *(elem++) = chainecur;
  }
  return liste;
}

/**
 * \brief  decodage des elements d'un argument 'liste diINTEGER' de lgr quelconque
 * @param label   pointer vers le label de la liste
 * @param number  taille de la liste  trouvee
 * @return   Pointer vers le tableau de lgr 'taille'
 *     Pointer NULL si erreur
 */
int *ArgMgr::ArgMgrGetListOfInt ( char *label, int *number )
{
  char *value = ArgMgrValue(label);
  int *liste;
  int *elem;
  int taille;
  if (!value)
     return 0;
  *number = IdStrCountChar(value,',')+1; /* nb Elements = nb Commas +1 */
  taille= *number;
  liste = (int *) calloc (1,sizeof(int)*taille );
  if ( !liste )
     return 0;
  elem = liste;
  *number = 1;

  while ( taille>0 ) 
  {
    *(elem++) = (int) strtol ( value, &value, 10 );      
    if ( *value == '\0' )
       return liste;
    if ( *(value++) != ',' ) 
    {
      free (liste);
      return 0;
    }
    taille --;
  }
return liste;
}

/**
 * \brief  decodage des elements d'un argument 'liste de FLOAT' de lgr qcq
 * @param label   pointer vers le label de la liste
 * @param number  taille de la liste  trouvee
 * @return   Pointer vers le tableau de lgr 'taille'
 *     NULL if error
 */
float *ArgMgr::ArgMgrGetListOfFloat ( char *label, int *number )
{
  char *value = ArgMgrValue(label);
  float *liste;
  float *elem;
  int taille;
  if (!value)
    return 0;
  *number = IdStrCountChar(value,',')+1; /* nb Elements = nb Commas +1 */
  taille= *number;
  liste = (float *) calloc (1,sizeof(float)*taille );
  if ( !liste )
     return 0;
  elem = liste;
  *number = 1;

  while ( taille>0 ) 
  {
    *(elem++) = (float) strtod ( value, &value );      
    if ( *value == '\0' )
       return liste;
    if ( *(value++) != ',' )
    {
      free (liste);
      return 0;
    }
    taille --;
  }
return liste;
}

/**
 * \brief     Counts the nb of occurrences of a given charact within a 'string' 
 * @param chaine     Pointer to the 'string'
 * @param caract     charact to count
 * @return       occurence number
 */
int ArgMgr::IdStrCountChar (char *chaine, int caract)
{
  int i=0;
  char *ptr;
  for ( ptr = chaine ; *ptr!='\0' ; ptr ++ ) 
     if (*ptr==caract) 
        i++;  
  return i;
}

/**
 * \brief     renvoie 1 tableau contenant une liste d'intervalles entiers
 * @param value     pointer vers la zone allouee contenant les
 *    intervalles (deb1,fin1, deb2,fin2, ...)
 * @param number     Pointer vers le nb d'interv trouves
 * @return        1 tableau contenant une liste d'intervalles entiers
 */
int *ArgMgr::IdStrIntEnum ( char* value, int *number)
{
   int* liste;
   int taille;
   int i;

   *number = IdStrCountChar(value,',')+1; /* nb Elements = nb Virgules +1 */
   taille= *number;
   liste = (int *) calloc (1,sizeof(int)*2*taille );
   if ( !liste )
   {
      return 0;
   }
   i=0;
   while ( taille>0 ) 
   {
      liste[i] = (int) strtol ( value, &value, 10 );
      if ( *value == '\0' ) 
      {
         liste[i+1]=liste[i];
         return liste;
      }
      if ( *(value++) != '-' ) 
      {
         liste[i+1]=liste[i];
         value--;
       }
       else
       {
          liste[i+1] = (int) strtol ( value, &value, 10 );
       }
       if ( *value == '\0' )
          return liste;
       if ( *(value++) != ',' )
       {
          free (liste);
          return 0;
       }
       taille --; i+=2;
   }
   return liste;
}

/**
 * \brief     renvoie 1 tableau contenant une liste d'intervalles FLOAT
 * @param value     pointer vers la zone allouee contenant les
 *     intervalles (deb1,fin1, deb2,fin2, ...)
 * @param number     Pointer vers le nb d'interv trouves
 * @return        1 tableau contenant une liste d'intervalles FLOAT
 */
float *ArgMgr::IdStrFloatEnum (char *value, int *number)
{
   float *liste;
   int taille;
   int i;
   *number = IdStrCountChar(value,',')+1; /* nb Elements = nb Virgules +1 */
   taille= *number;
   liste = (float *) calloc (1,sizeof(float)*2*taille );
   if ( !liste )
      return 0;
   i=0;
   while ( taille>0 ) 
   {
      liste[i] = (float) strtod ( value, &value );      
      if ( *value == '\0' ) 
      {
         liste[i+1]=liste[i];
         return liste;
      }
      if ( *(value++) != '-' ) 
      {
         liste[i+1]=liste[i];
         value--;
      }
      else
      {
          liste[i+1] = (float) strtod ( value, &value );
      }
      if ( *value == '\0' ) 
         return liste;
      if ( *(value++) != ',' )
      {
         free (liste);
         return 0;
      }
      taille --; i+=2;
   }
   return liste;
}

/**
 * \brief  decodage des elements d'un argument 'intervalles d'int' de lgr quelconque
 * @param label   pointer vers le label de la liste
 * @param number  taille de la liste d'intervalles trouvee
 * @return   Pointer vers le tableau de taille '2*nbElem'
 *     Pointer NULL si erreur
 */
int *ArgMgr::ArgMgrGetIntEnum ( char *label, int *number )
{
   char *value = ArgMgrValue(label);
   int *liste;
   if (!value) 
      return 0;
   liste = IdStrIntEnum(value, number);
   return liste;
}

/**
 * \brief  decodage des elements d'un argument 'intervalles d'int' de lgr quelconque
 * @param label   pointer vers le label de la liste
 * @param number  taille de la liste d'intervalles trouvee
 * @return   Pointer vers le tableau de taille '2*nbElem'
 *     Pointer NULL si erreur
 */
float *ArgMgr::ArgMgrGetFloatEnum ( char *label, int *number )
{
   char  *value = ArgMgrValue(label);
   float *liste;
   if (!value) 
      return 0;
   liste = IdStrFloatEnum(value, number);
   return liste;
}

//-----------------------------------------------------------------------------
// Protected

//-----------------------------------------------------------------------------
// Private

/**************************************************************************
*                                                                         *
* Nom de la fonction : Majuscule                                          *
* Role ............. : Met une chaine de caracteres en majuscules.        *
* parameters ....... : Pointer vers la chaine.                           *
* Valeur retournee . : pointer vers cette meme chaine en majuscule.      *
*                                                                         *
**************************************************************************/
char *ArgMgr::Majuscule (char *chaine )
{
  char *ptr, *ptr2, *ptr3;
  ptr2 = (char *)malloc(strlen(chaine)*sizeof(char)+1);
  ptr3=ptr2;
  for ( ptr = chaine ; *ptr!='\0' ; ptr ++ ) 
   {  
       *ptr3 = toupper ( * ptr ); ptr3++; 
   }
  *ptr3='\0'; 
  return ptr2;
}

/**************************************************************************
*                                                                         *
* Nom de la fonction : FiltreLong                                         *
* Role ............. : Arrete le programme si l'argument est trop long.   *
*                      ARG_LONG_MAX definit cette longueur.               *
* parameters ....... : Pointer vers l'argument.                          *
* Valeur retournee . : Faux s'il n'y a pas d'erreur.                      *
*                      Vrai s'il y a une erreur.                          *
**************************************************************************/
int ArgMgr::FiltreLong ( char *arg  )
{
  int  n = 0 ;
  while ( (n++<ARG_LONG_MAX) && (*(arg++) != '\0') ) ;
  return (n>=ARG_LONG_MAX) ;
}

/*------------------------------------------------------------------------
 | Role       : Lit un parameter depuis un fichier
 | Retour     : Type   : char *
 |              Role   : pointer vers le label
 | parameters : param  : char *
 |              Role   : one ou il faut stocker le parameter
 |              fd     : FILE *
 |              Role   : descripteur du fichier (suppose ouvert)
 +------------------------------------------------------------------------*/
char *ArgMgr::LoadedParam ( char *param, FILE *fd )
{
  int    carlu;
  char * car = param;
  int    quote = false;
  int    nbcar = 0;

  /* On supprime les espaces ****/
  /* du debut de chaine *********/
  while ( isspace(carlu=fgetc (fd)) );
  if (carlu==EOF)
     return 0;
  /* On cherche une " eventuelle */
  if ( carlu=='\"' ) 
  {
    carlu=fgetc(fd);
    quote=true;
  /* On lit tous les caracteres */
  }
  while (  (carlu!=EOF)
        && (  ( (!quote)&&(!isspace(carlu)) )
         ||( (quote)&& !(carlu=='\"')   ) ) ) 
  {
     *(car++) = (char) carlu;
     nbcar ++;
  /* sans depasser la taille max*/
     if ( nbcar >= ARG_LONG_MAX ) 
     {
        std::cout << "\nError: Argument too long ( > "
                  << ARG_LONG_MAX << ")in parameter file."
                  << std::endl;
        break;
     }
     carlu = fgetc(fd);
  }
  *car = '\0';
  return param;
}

/*------------------------------------------------------------------------
 | Role       : Lecture d'arguments dans un fichier de parameters
 |              (cette fonction est recursive).
 | Retour     : Type   : int
 |              Role   : retourne le nombre de lettres necessaires
 |                       pour stocker en memoire tous les parameters
 | parameters : filename : char *
 |              Role     : nom du fichier de parameter
 |
 +------------------------------------------------------------------------*/
int ArgMgr::ArgLoadFromFile ( char *filename )
{
  int   nbl = 0;
  char  param[ARG_LONG_MAX+1];
  FILE  *fch;

  fch = fopen ( filename, ID_RFILE_TEXT );
  while ( LoadedParam (param, fch ) )
  {
    int n = strlen(param);
    if ( param[0]=='@' )
    {
      nbl  += ArgLoadFromFile ( &param[1] );
    }
    else
    {
      ArgLab [ArgCount] = strcpy ((char *) malloc(n+1), param ) ;
      nbl += n + 1 ;
      ArgCount++;
      if ( ArgCount >= ARGMAXCOUNT ) 
         break;
    }
  }
  fclose ( fch );
  return nbl;
}

/*------------------------------------------------------------------------
 | Role       : Gestion des parameters standard de la ligne d'argument.
 | Retour     : Type   : void
 | parameters : aucun
 +------------------------------------------------------------------------*/
void ArgMgr::ArgStdArgs()
{
  char *logfile;
  FILE *fd;

  if ( (ArgParamOut=ArgMgrValue(ARG_LABEL_PARAMOUT))==0 )
    ArgParamOut = ARG_DEFAULT_PARAMOUT;
  if ( (logfile = ArgMgrValue(ARG_LABEL_LOGFILE))!=0) 
  {
    if ( *logfile == '\0' )
       logfile = ARG_DEFAULT_LOGFILE;
    fd = fopen ( logfile, "a+" );
    if ( fd ) 
    {
      fprintf ( fd, "%s\n", Appel );
      fclose  ( fd );
    }
  }
}

/*------------------------------------------------------------------------
 | Role       : met en majuscule 'sur place'.
 | Retour     : Type   : char *
 | parameters : char *
 +------------------------------------------------------------------------*/
char *ArgMgr::maj ( char *a )
{
   char *b = a;
   while ( *b !=0 ) 
   {
      if ( *b<='z' && *b>='a' ) *b = *b+'A'-'a';
      b++;
   }
   return a;
}
//-----------------------------------------------------------------------------
// Print

//-----------------------------------------------------------------------------
} // end namespace gdcm
