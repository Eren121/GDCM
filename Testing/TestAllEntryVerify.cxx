#include <map>
#include <list>
#include <fstream>
#include <iostream>
#include "gdcmHeader.h"

//Generated file:
#include "gdcmDataImages.h"

using namespace std;

typedef string EntryValueType;   // same type as gdcmValEntry::value
typedef list<gdcmTagKey, EntryValueType> ListEntryValues;
typedef string FileNameType;
typedef map<FileNameType, ListEntryValues*> MapFileValuesType;

class ReferenceFileParser
{
   istream& eatwhite(istream& is);
   void eatwhite(string& toClean);
   string ExtractFirstString(string& toSplit);
   string ExtractValue(string& toSplit);
   void CleanUpLine( string& line );
   void DisplayLine();

   bool FirstPassReferenceFile();
   bool SecondPassReferenceFile();
   bool HandleFileName( string& line );
   bool HandleKey( string& line );
   bool HandleValue( string& line );
public:
   ReferenceFileParser();
   bool Open( string& referenceFileName );
private:
   /// The ifstream attached to the file we parse:
   ifstream from;

   /// True when things went wrong, false otherwise
   bool ParsingFailed;
   
   /// String prefixing every output
   string Indent;

   /// The current line position within the stream:
   int lineNumber;

   string CurrentFileName;
   string CurrentKey;
   string CurrentValue;
   string CurrentErrorMessage;
};

istream& ReferenceFileParser::eatwhite( istream& is )
{
   char c;
   while (is.get(c)) {
      if (!isspace(c)) {
         is.putback(c);
         break;
      }
   }
   return is;
}

void ReferenceFileParser::eatwhite( string& toClean )
{
   while( toClean.find_first_of( " " ) == 0  )
      toClean.erase( 0, toClean.find_first_of( " " ) + 1 );
}

string ReferenceFileParser::ExtractFirstString( string& toSplit )
{
   std::string firstString;
   eatwhite( toSplit );
   if ( toSplit.find( " " ) == string::npos ) {
      firstString = toSplit;
      toSplit.erase();
      return firstString;
   }
   firstString = toSplit.substr( 0, toSplit.find(" ") );
   toSplit.erase( 0, toSplit.find(" ") + 1);
   eatwhite( toSplit );
   return firstString;
}

string ReferenceFileParser::ExtractValue( string& toSplit )
{
   eatwhite( toSplit );
   string::size_type beginPos = toSplit.find_first_of( '"' );
   string::size_type   endPos = toSplit.find_last_of( '"' );

   // Make sure we have at most to " in toSplit:
   string noQuotes = toSplit.substr( beginPos + 1, endPos - beginPos - 1);
   if ( noQuotes.find_first_of( '"' ) != string::npos )
   {
      CurrentErrorMessage = "more than two quote character";
      return string();
   }

   // No leading quote means this is not a value:
   if ( beginPos == string::npos )
   {
      return string();
   }

   if ( ( endPos == string::npos ) || ( beginPos == endPos ) )
   {
      CurrentErrorMessage = "unmatched \" (quote character)";
      return string();
   }

   if ( beginPos != 0 )
   {
      ostringstream error;
      error  << "leading character ["
             << toSplit.substr(beginPos -1, 1)
             << "] before opening \" ";
      CurrentErrorMessage = error.str();
      return string();
   }

   // When they are some extra characters at end of value, it must
   // be a space:
   if (   ( endPos != toSplit.length() - 1 )
       && ( toSplit.substr(endPos + 1, 1) != " " ) )
   {
      ostringstream error;
      error  << "trailing character ["
             << toSplit.substr(endPos + 1, 1)
             << "] after value closing \" ";
      CurrentErrorMessage = error.str();
      return string();
   }

   string value = toSplit.substr( beginPos + 1, endPos - beginPos - 1 );
   toSplit.erase(  beginPos, endPos - beginPos + 1);
   eatwhite( toSplit );
   return value;
}

/// \brief   Checks the block syntax of the incoming ifstream. Checks that
///          - no nested blocks are present
///          - we encounter a matching succesion of "[" and "]"
///          - when ifstream terminates the last block is closed.
///          - a block is not opened and close on the same line
/// @param   from The incoming ifstream to be checked.
/// @return  True when incoming ifstream has a correct syntax, false otherwise.
/// \warning The underlying file pointer is not preseved.
bool ReferenceFileParser::FirstPassReferenceFile()
{
   string line;
   lineNumber = 1;
   bool inBlock = false;
   from.seekg( 0, ios::beg );

   while ( ! from.eof() )
   {
      getline( from, line );

      /// This is how we usually end the parsing because we hit EOF:
      if ( ! from.good() )
      {
         if ( ! inBlock )
            break;
         else
         {
           cerr << Indent << "Syntax error: EOF reached when in block." << endl;
           ParsingFailed = true;
           break;
         }
      }

      // Don't try to parse comments (weed out anything after first "#"):
      if ( line.find_first_of( "#" ) != string::npos )
      {
         line.erase( line.find_first_of( "#" ) );
      }

      // Two occurences of opening blocks on a single line implies nested
      // blocks which is illegal:
      if ( line.find_first_of( "[" ) != line.find_last_of( "[" ) )
      {
         cerr << Indent
              << "Syntax error: nested block (open) in reference file"
              << endl
              << Indent << "at line " << lineNumber << endl;
         ParsingFailed = true;
         break;
      }

      // Two occurences of closing blocks on a single line implies nested
      // blocks which is illegal:
      if ( line.find_first_of( "]" ) != line.find_last_of( "]" ) )
      {
         cerr << Indent
              << "Syntax error: nested block (close) in reference file"
              << endl
              <<  Indent << "at line " << lineNumber << endl;
         ParsingFailed = true;
         break;
      }

      bool beginBlock ( line.find_first_of("[") != string::npos );
      bool endBlock   ( line.find_last_of("]")  != string::npos );

      // Opening and closing of block on same line:
      if ( beginBlock && endBlock )
      {
         cerr << Indent
              << "Syntax error: opening and closing on block on same line "
              << lineNumber++ << endl;
         ParsingFailed = true;
         break;
      }

      // Illegal closing block when block not open:
      if ( !inBlock && endBlock )
      {
         cerr << Indent << "Syntax error: unexpected end of block at line "
              << lineNumber++ << endl;
         ParsingFailed = true;
         break;
      }
  
      // Uncommented line outside of block is not clean:
      if ( !inBlock && !beginBlock )
      {
         cout << Indent
              << "Syntax warning: outside of block [] data at line "
              << lineNumber++ << " not considered." << endl;
         continue;
      }

      if ( inBlock && beginBlock )
      {
         cerr << "   Syntax error: illegal opening of nested block at line "
              << lineNumber++ << endl;
         ParsingFailed = true;
         break;
      }

      // Normal situation of opening block:
      if ( beginBlock )
      {
         inBlock = true;
         lineNumber++;
         continue;
      }

      // Normal situation of closing block:
      if ( endBlock )
      {
         inBlock = false;
         lineNumber++;
         continue;
      }
      // This line had no block delimiter
      lineNumber++;
   }

   // We need rewinding:
   from.clear();
   from.seekg( 0, ios::beg );

   return ! ParsingFailed;
}

ReferenceFileParser::ReferenceFileParser()
{
   lineNumber = 1;
   ParsingFailed = false;
   Indent = "      ";
}

bool ReferenceFileParser::Open( string& referenceFileName )
{
   from.open( referenceFileName.c_str(), ios::in );
   if ( !from.is_open() )
   {
      cerr << Indent << "Can't open reference file." << endl;
      ParsingFailed = true;
   }
   
   if ( !FirstPassReferenceFile() )
      return false;

   SecondPassReferenceFile();
   from.close();
}

void ReferenceFileParser::CleanUpLine( string& line )
{
   // Cleanup from comments:
   if ( line.find_first_of( "#" ) != string::npos )
      line.erase( line.find_first_of( "#" ) );

   // Cleanup everything after end block delimiter:
   if ( line.find_last_of( "]" ) != string::npos )
      line.erase( line.find_last_of( "]" ) + 1 );

   // Cleanup leanding whites and skip empty lines:
   eatwhite( line );
}

bool ReferenceFileParser::HandleFileName( string& line )
{
   if ( line.length() == 0 )
   {
      CurrentErrorMessage = "unfound filename";
      return false;
   }
   if ( CurrentFileName.length() != 0 )
      return true;
   CurrentFileName = ExtractFirstString(line);
   cout << Indent << "Found filename " << CurrentFileName << endl;
   return true;
}

bool ReferenceFileParser::HandleKey( string& line )
{
   if ( line.length() == 0 )
      return false;

   if ( CurrentKey.length() != 0 )
      return false;

   CurrentKey = ExtractFirstString(line);
   if ( CurrentKey.find_first_of( "|" ) == string::npos )
   {
      ostringstream error;
      error  << "uncorrect key:" << CurrentKey;
      CurrentErrorMessage = error.str();
      return false;
   }
   cout << Indent << "Found key:" << CurrentKey << endl;
   return true;
}

bool ReferenceFileParser::HandleValue( string& line )
{
   if ( line.length() == 0 )
   {
      CurrentErrorMessage = "no value present";
      return false;
   }

   if ( CurrentKey.length() == 0 )
   {
      cout << Indent << "No key present:" << CurrentKey << endl;
      return false;
   }
   
   string newCurrentValue = ExtractValue(line);
   if ( newCurrentValue.length() == 0 )
   {
      if ( CurrentErrorMessage.length() == 0 )
      {
         ostringstream error;
         error  << "missing value for key:" << CurrentKey;
         CurrentErrorMessage = error.str();
      }
      return false;
   }

   cout << Indent << "Found value:" << newCurrentValue << endl;
   CurrentValue += newCurrentValue;
   return true;
}

void ReferenceFileParser::DisplayLine()
{
      cerr << Indent << "Syntax error at line " << lineNumber
           << ": " << CurrentErrorMessage << "." << endl;
}

bool ReferenceFileParser::SecondPassReferenceFile()
{
   gdcmTagKey key;
   EntryValueType value;
   string line;
   bool inBlock = false;
   lineNumber = 0;

   while ( !from.eof() )
   {
      getline( from, line );
      lineNumber++;

      CleanUpLine( line );

      // Empty lines don't require any treatement:
      if ( line.length() == 0 )
         continue;

      bool beginBlock ( line.find_first_of("[") != string::npos );
      bool endBlock   ( line.find_last_of("]")  != string::npos );

      // Waiting for a block to be opened. Meanwhile, drop everything:
      if ( !inBlock && !beginBlock )
         continue;

      if ( beginBlock )
      {
         cout << Indent << "Begin block" << endl;
         inBlock = true;
         line.erase( 0, line.find_first_of( "[" ) + 1 );
         eatwhite( line );
      }
      else if ( endBlock )
      {
         cout << Indent << "Detected End block" << endl;
         line.erase( line.find_last_of( "]" ) );
         eatwhite( line );
      }

      // Outside block lines are dropped:
      if ( ! inBlock )
         continue;

      cout << Indent << "Default case:" << line << endl;

      // First thing is to get a filename:
      if ( ( ! HandleFileName( line ) ) && ( line.length() != 0 ) )
      {
         DisplayLine();
         return false;
      }

      // Second thing is to get a key:
      if ( ( ! HandleKey( line ) ) && ( line.length() != 0 ) )
      {
         ParsingFailed = true;
      }
       
      // Third thing is to get a value:
      if (    ( ! ParsingFailed )
          &&  ( ! HandleValue( line ) )
          && ( line.length() != 0) )
      {
         ParsingFailed = true;
      }

      if (   ( ! ParsingFailed )
          && CurrentKey.length()
          && CurrentValue.length() )
      {
         cout << Indent << "Need to handle pair ("
              << CurrentKey << ", " << CurrentValue << ")." << endl;
         CurrentKey.erase();
         CurrentValue.erase();
      }

      if ( ParsingFailed )
      {
         DisplayLine();
         return false;
      }

      if ( endBlock )
      {
         cout << Indent << "Need to handle end of block " << endl;
         inBlock = false;
         CurrentFileName.erase();
      }

      //key     = TranslateToKey(group, element);
      ///tb1[key] = name.c_str();
      //cout << group << element << vr << fourth << name;
      //tb1[key] = "bozo";
   }
}

int TestAllEntryVerify(int argc, char* argv[]) 
{
   if ( argc > 1 )
   {
      cerr << "   Usage: " << argv[0]
                << " (no arguments needed)." << endl;
      return 1;
   }
   
   cout << "   Description (Test::TestAllEntryVerify): "
             << endl;
   cout << "   For all images in gdcmData (and not blacklisted in "
                "Test/CMakeLists.txt)"
             << endl;
   cout << "   apply the following to each filename.xxx: "
             << endl;
   cout << "   step 1: parse the image (as gdcmHeader) and call"
             << " IsReadable(). "
             << endl;

   string referenceFilename = GDCM_DATA_ROOT;
   referenceFilename += "/";  //doh!
   referenceFilename += "TestAllEntryVerifyReference.txt";

   ReferenceFileParser Parser;
   Parser.Open(referenceFilename);
/*
   int i = 0;
   while( gdcmDataImages[i] != 0 )
   {
      string filename = GDCM_DATA_ROOT;
      filename += "/";  //doh!
      filename += gdcmDataImages[i++];
   
      cout << "   Testing: " << filename << endl;

      gdcmHeader* tested = new gdcmHeader( filename.c_str(), false, true );
      if( !tested->GetHeader()->IsReadable() )
      {
        cout << "      Image not gdcm compatible:"
                  << filename << endl;
        delete tested;
        return 1;
      }

      //////////////// Clean up:
      delete tested;
   }
*/

   return 0;
}
