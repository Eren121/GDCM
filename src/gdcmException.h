// gdcmException.h
//-----------------------------------------------------------------------------
#ifndef GDCM_EXCEPTION_H
#define GDCM_EXCEPTION_H

#include "gdcmCommon.h"
#include <string>
#include <iostream>
#include <exception>

//-----------------------------------------------------------------------------
/*
 * Any exception thrown in the gdcm library
 */
class GDCM_EXPORT gdcmException : public std::exception {
public:
   /*
    * Builds an exception with minimal information: name of the thrower
    * method and error message
    *
    * @param from name of the thrower
    * @param error error description string
    */
   explicit gdcmException(const std::string &from, const std::string &error = "")
    throw();

   /*
    * virtual destructor makes this class dynamic
    */
   virtual ~gdcmException() throw() {
   }

   // exception caught within exception class: print error message and die
   static void fatal(const char *from) throw();

   // returns error message
   const std::string &getError(void) const throw() {
    return error;
   }

   // try to discover this (dynamic) class name
   virtual std::string getName() const throw();

   // returns exception name string (overloads std::exception::what)
   virtual const char *what() const throw() {
    return (const char *) *this;
   }

   // returns exception name string
   operator const char *() const throw();

   friend std::ostream& operator<<(std::ostream &os, const gdcmException &e);

protected:
   // error message
   std::string from;
   // error message
   std::string error;
};


//-----------------------------------------------------------------------------
/*
 * File error exception thrown in the gdcm library
 */
class GDCM_EXPORT gdcmFileError : public gdcmException {
public:
   /*
    * Builds an file-related exception with minimal information: name of
    * the thrower method and error message
    *
    * @param from name of the thrower
    * @param error error description string
    */
   explicit gdcmFileError(const std::string &from,
                          const std::string &error = "File error")
      throw() : gdcmException(from, error) {
   }
};


//-----------------------------------------------------------------------------
/*
 * Invalid file format exception
 */
class GDCM_EXPORT gdcmFormatError : public gdcmException {
public:
   /*
    * Builds an file-related exception with minimal information: name of
    * the thrower method and error message
    *
    * @param from name of the thrower
    * @param error error description string
    */
   explicit gdcmFormatError(const std::string &from,
                            const std::string &error = "Invalid file format error")
      throw() : gdcmException(from, error) {
   }
};

//-----------------------------------------------------------------------------
/* prints exception stack on output stream
 * @param os output stream
 * @param e exception to print
 * @returns output stream os
 */
std::ostream& operator<<(std::ostream &os, const gdcmException &e);

//-----------------------------------------------------------------------------
#endif // GDCM_EXCEPTION_H
