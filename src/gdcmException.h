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

#ifndef GDCM_EXCEPTION_H
#define GDCM_EXCEPTION_H

#include <string>
#ifdef _MSC_VER
using namespace std;  // string type lives in the std namespace on VC++
#endif

#include <iostream>
#include <exception>
using namespace std;

#ifdef _MSC_VER
#define GDCM_EXPORT __declspec( dllexport )
#else
#define GDCM_EXPORT
#endif

/**
 * Any exception thrown in the gdcm library
 */
class GDCM_EXPORT gdcmException : public exception {
 protected:
  /// error message
  string from;
  /// error message
  string error;

 public:
  /**
   * Builds an exception with minimal information: name of the thrower
   * method and error message
   *
   * @param from name of the thrower
   * @param error error description string
   */
  explicit gdcmException(const string &from, const string &error = "")
    throw();
  

  /**
   * virtual destructor makes this class dynamic
   */
  virtual ~gdcmException() {
  }
  
  /// returns error message
  const string &getError(void) const throw() {
    return error;
  }

  /// returns exception name string
  operator const char *() const throw();

  /// returns exception name string (overloads std::exception::what)
  virtual const char *what() const throw() {
    return (const char *) *this;
  }


  /// exception caught within exception class: print error message and die
  static void fatal(const char *from) throw();

  /// try to discover this (dynamic) class name
  virtual string getName() const throw();

  friend ostream& operator<<(ostream &os, const gdcmException &e);
  
};


/** prints exception stack on output stream
 * @param os output stream
 * @param e exception to print
 * @returns output stream os
 */
ostream& operator<<(ostream &os, const gdcmException &e);


/**
 * File error exception thrown in the gdcm library
 */
class GDCM_EXPORT gdcmFileError : public gdcmException {
 public:
  /**
   * Builds an file-related exception with minimal information: name of
   * the thrower method and error message
   *
   * @param from name of the thrower
   * @param error error description string
   */
  explicit gdcmFileError(const string &from,
			 const string &error = "File error")
    throw() : gdcmException(from, error) {
  }
};




/**
 * Invalid file format exception
 */
class GDCM_EXPORT gdcmFormatError : public gdcmException {
 public:
  /**
   * Builds an file-related exception with minimal information: name of
   * the thrower method and error message
   *
   * @param from name of the thrower
   * @param error error description string
   */
  explicit gdcmFormatError(const string &from,
			   const string &error = "Invalid file format error")
    throw() : gdcmException(from, error) {
  }
};


#endif // GDCM_EXCEPTION_H
