// gdcmUtil.h
//-----------------------------------------------------------------------------
#ifndef GDCMUTIL_H
#define GDCMUTIL_H

#include <vector>
#include <string>

/**
 * \defgroup Globals Utility functions
 * \brief    Here are some utility functions, belonging to NO class,
 *           dealing with strings, file names... that can be called
 *           from anywhere by whomsoever they can help.
 */

//-----------------------------------------------------------------------------
std::istream & eatwhite(std::istream & is);

void Tokenize (const std::string& str,
               std::vector<std::string>& tokens,
               const std::string& delimiters = " ");

std::string CreateCleanString(std::string s);
void        NormalizePath(std::string &name);
std::string GetPath(std::string &fullName);
std::string GetName(std::string &fullName);
//-----------------------------------------------------------------------------
#endif
