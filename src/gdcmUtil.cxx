// gdcmUtil.cxx
//-----------------------------------------------------------------------------
#include "gdcmUtil.h"

#include <stdio.h>
#include <ctype.h>   // For isspace
#include <string.h>

//-----------------------------------------------------------------------------
// Library globals.
gdcmDebug dbg;

//-----------------------------------------------------------------------------
/**
 * \ingroup gdcmDebug
 * \brief   constructor
 * @param level debug level
 */ 

gdcmDebug::gdcmDebug(int level) {
   DebugLevel = level;
}

/**
 * \ingroup gdcmDebug
 * \brief   Verbose 
 * @param Level level
 * @param Msg1 first message part
 * @param Msg2 second message part 
 */
void gdcmDebug::Verbose(int Level, const char * Msg1, const char * Msg2) {
   if (Level > DebugLevel)
      return ;
   std::cerr << Msg1 << ' ' << Msg2 << std::endl;
}

/**
 * \ingroup gdcmDebug
 * \brief   Error 
 * @param Test test
 * @param Msg1 first message part
 * @param Msg2 second message part 
 */
void gdcmDebug::Error( bool Test, const char * Msg1, const char * Msg2) {
   if (!Test)
      return;
   std::cerr << Msg1 << ' ' << Msg2 << std::endl;
   Exit(1);
}

/**
 * \ingroup gdcmDebug
 * \brief   Error 
 * @param Msg1 first message part
 * @param Msg2 second message part
 * @param Msg3 Third message part  
 */
void gdcmDebug::Error(const char* Msg1, const char* Msg2,
                      const char* Msg3) {
   std::cerr << Msg1 << ' ' << Msg2 << ' ' << Msg3 << std::endl;
   Exit(1);
}

/**
 * \ingroup gdcmDebug
 * \brief   Assert 
 * @param Level level 
 * @param Test test
 * @param Msg1 first message part
 * @param Msg2 second message part
 */
 void gdcmDebug::Assert(int Level, bool Test,
                 const char * Msg1, const char * Msg2) {
   if (Level > DebugLevel)
      return ;
   if (!Test)
      std::cerr << Msg1 << ' ' << Msg2 << std::endl;
}

/**
 * \ingroup gdcmDebug
 * \brief   Exit 
 * @param a return code 
 */
void gdcmDebug::Exit(int a) {
#ifdef __GNUC__
   std::exit(a);
#endif
#ifdef _MSC_VER
   exit(a);    // Found in #include <stdlib.h>
#endif
}

//-----------------------------------------------------------------------------
/// Pointer to a container, holding *all* the Dicom Dictionaries
gdcmDictSet         *gdcmGlobal::Dicts  = (gdcmDictSet *)0;
/// Pointer to a H table containing the 'Value Representations'
gdcmVR              *gdcmGlobal::VR     = (gdcmVR *)0;
/// Pointer to a H table containing the Transfer Syntax codes and their english description 
gdcmTS              *gdcmGlobal::TS     = (gdcmTS *)0;
/// Pointer to a H table containing the Dicom Elements necessary to describe each part of a DICOMDIR 
gdcmDicomDirElement *gdcmGlobal::ddElem = (gdcmDicomDirElement *)0;
/// gdcm Glob
gdcmGlobal gdcmGlob;


/**
 * \ingroup gdcmGlobal
 * \brief   constructor : populates the
 */
gdcmGlobal::gdcmGlobal(void) {
   if (VR || TS || Dicts || ddElem)
      dbg.Verbose(0, "gdcmGlobal::gdcmGlobal : VR or TS or Dicts already allocated");
   Dicts  = new gdcmDictSet();
   VR     = new gdcmVR();
   TS     = new gdcmTS();
   ddElem = new gdcmDicomDirElement();
}

/**
 * \ingroup gdcmGlobal
 * \brief   canonical destructor 
 */
gdcmGlobal::~gdcmGlobal() {
   delete Dicts;
   delete VR;
   delete TS;
   delete ddElem;
}

gdcmVR *gdcmGlobal::GetVR(void) {
   return VR;
}

gdcmTS *gdcmGlobal::GetTS(void) {
   return TS;
}

gdcmDictSet *gdcmGlobal::GetDicts(void) {
   return Dicts;
}

gdcmDicomDirElement *gdcmGlobal::GetDicomDirElements(void) {
   return ddElem;
}

//-----------------------------------------------------------------------------
// Here are some usefull functions, belonging to NO class,
// dealing with strings, file names, etc
// that can be called from anywhere
// by whomsoever they can help.
//-----------------------------------------------------------------------------

// Because is not yet available in g++2.96
std::istream& eatwhite(std::istream& is) {
   char c;
   while (is.get(c)) {
      if (!isspace(c)) {
         is.putback(c);
         break;
      }
   }
   return is;
}

///////////////////////////////////////////////////////////////////////////
// Because is not  available in C++ (?)
void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters) {
   std::string::size_type lastPos = str.find_first_not_of(delimiters,0);
   std::string::size_type pos     = str.find_first_of    (delimiters,lastPos);
   while (std::string::npos != pos || std::string::npos != lastPos) {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delimiters, pos);
      pos     = str.find_first_of    (delimiters, lastPos);
   }
}


///////////////////////////////////////////////////////////////////////////
/**
 * \brief  to prevent a flashing screen when non-printable character
 * @param v characters array to remove non printable characters from
 */
char *_cleanString(char *v) {
   char *d;
   int i, l;
   l = strlen(v);
   for (i=0,d=v; 
      i<l ; 
      i++,d++) {
         if (!isprint(*d))
         *d = '.';
   }	
   return v;
}

///////////////////////////////////////////////////////////////////////////
/**
 * \brief   to prevent a flashing screen when non-printable character
 * @param s string to remove non printable characters from
 */
std::string _CreateCleanString(std::string s) {
   std::string str=s;

   for(int i=0;i<str.size();i++)
   {
      if(!isprint(str[i]))
         str[i]='.';
   }


   if(str.size()>0)
      if(!isprint(s[str.size()-1]))
         if(s[str.size()-1]==0)
            str[str.size()-1]=' ';

   return(str);
}

///////////////////////////////////////////////////////////////////////////
/**
 * \brief   Add a SEPARATOR to the end of the name is necessary
 * @param name file/directory name to normalize 
 */
void NormalizePath(std::string &name)
{
const char SEPARATOR_X      = '/';
const char SEPARATOR_WIN    = '\\';
const std::string SEPARATOR = "/";
   int size=name.size();
   if((name[size-1]!=SEPARATOR_X)&&(name[size-1]!=SEPARATOR_WIN))
   {
      name+=SEPARATOR;
   }
}

///////////////////////////////////////////////////////////////////////////
/**
 * \brief   Get the (directory) path from a full path file name
 * @param fullName file/directory name to extract Path from
 */
std::string GetPath(std::string &fullName)
{
   int pos1=fullName.rfind("/");
   int pos2=fullName.rfind("\\");
   if(pos1>pos2)
      fullName.resize(pos1);
   else
      fullName.resize(pos2);
   return(fullName);
}

///////////////////////////////////////////////////////////////////////////
/**
 * \brief   Get the (last) name of a full path file name
 * @param fullName file/directory name to extract end name from
 */
std::string GetName(std::string &fullName)
{   
  int fin=fullName.length()-1;
  char a =fullName.c_str()[fin];
  if (a == '/' || a == '\\') {
     fin--;
  }
  int deb;
  for (int i=fin;i!=0;i--) {
     if (fullName.c_str()[i] == '/' || fullName.c_str()[i] == '\\')  
        break;
      deb = i;
  }    

  std::string lastName;
  for (int j=deb;j<fin+1;j++)
    lastName=lastName+fullName.c_str()[j];

  return(lastName);
} 
