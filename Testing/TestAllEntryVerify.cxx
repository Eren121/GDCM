#include <map>
#include <list>
#include <fstream>
#include <iostream>
#include "gdcmHeader.h"

//Generated file:
#include "gdcmDataImages.h"

using namespace std;

typedef string EntryValueType;   // same type as gdcmValEntry::value
typedef map< gdcmTagKey, EntryValueType > MapEntryValues;
typedef MapEntryValues* MapEntryValuesPtr;
typedef string FileNameType;
typedef map< FileNameType, MapEntryValuesPtr > MapFileValuesType;

struct ParserException
{
   string error;
   static string Indent;

   static string GetIndent() { return ParserException::Indent; }
   ParserException( string ErrorMessage )
   {
      error = ErrorMessage;
      Indent = "      ";
   }
   void Print() { cerr << Indent << error << endl; }
};

string ParserException::Indent = "      ";

class ReferenceFileParser
{
   bool AddKeyValuePairToMap( string& key, string& value );

   istream& eatwhite(istream& is);
   void eatwhite(string& toClean);
   string ExtractFirstString(string& toSplit);
   void CleanUpLine( string& line );

   string ExtractValue(string& toSplit)  throw ( ParserException );
   void ParseRegularLine( string& line ) throw ( ParserException );
   void FirstPassReferenceFile()         throw ( ParserException );
   bool SecondPassReferenceFile()        throw ( ParserException );
   void HandleFileName( string& line )   throw ( ParserException );
   void HandleKey( string& line )        throw ( ParserException );
   bool HandleValue( string& line )      throw ( ParserException );
   static uint16_t axtoi( char* );
public:
   ReferenceFileParser();
   bool Open( string& referenceFileName );
   void Print();
   void SetDataPath(string&);
   bool Check();
private:
   /// The directory containing the images to check:
   string DataPath;

   /// The product of the parser:
   MapFileValuesType ProducedMap;

   /// The ifstream attached to the file we parse:
   ifstream from;

   /// String prefixing every output
   string Indent;

   /// The current line position within the stream:
   int lineNumber;

   /// The currently parsed filename:
   string CurrentFileName;

   /// The currently parsed key:
   string CurrentKey;

   /// The currently parsed value:
   string CurrentValue;

   /// The current MapEntryValues pointer:
   MapEntryValues* CurrentMapEntryValuesPtr;
};

/// As gotten from:
/// http://community.borland.com/article/0,1410,17203,0.html
uint16_t ReferenceFileParser::axtoi(char *hexStg) {
  int n = 0;         // position in string
  int m = 0;         // position in digit[] to shift
  int count;         // loop index
  int intValue = 0;  // integer value of hex string
  int digit[5];      // hold values to convert
  while (n < 4) {
     if (hexStg[n]=='\0')
        break;
     if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
        digit[n] = hexStg[n] & 0x0f;            //convert to int
     else if (hexStg[n] >='a' && hexStg[n] <= 'f') //if a to f
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else if (hexStg[n] >='A' && hexStg[n] <= 'F') //if A to F
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
     else break;
    n++;
  }
  count = n;
  m = n - 1;
  n = 0;
  while(n < count) {
     // digit[n] is value of hex digit at position n
     // (m << 2) is the number of positions to shift
     // OR the bits into return value
     intValue = intValue | (digit[n] << (m << 2));
     m--;   // adjust the position to set
     n++;   // next digit to process
  }
  return (intValue);
}

void ReferenceFileParser::SetDataPath( string& inDataPath )
{
   DataPath = inDataPath;
}

bool ReferenceFileParser::AddKeyValuePairToMap( string& key, string& value )
{
   if ( !CurrentMapEntryValuesPtr )
      return false;
   if ( CurrentMapEntryValuesPtr->count(key) != 0 )
      return false;
   (*CurrentMapEntryValuesPtr)[key] = value;
   
   return true; //??
}

void ReferenceFileParser::Print()
{
   for (MapFileValuesType::iterator i  = ProducedMap.begin();
                                    i != ProducedMap.end();
                                    ++i)
   {
      cout << Indent << "FileName: " << i->first << endl;
      MapEntryValuesPtr KeyValues = i->second;
      for (MapEntryValues::iterator j  = KeyValues->begin();
                                    j != KeyValues->end();
                                    ++j)
      {
         cout << Indent
              << "  Key: " << j->first
              << "  Value: " << j->second
              << endl;
      }
      cout << Indent << endl;
   }
   cout << Indent << endl;
}

bool ReferenceFileParser::Check()
{
   for (MapFileValuesType::iterator i  = ProducedMap.begin();
                                    i != ProducedMap.end();
                                    ++i)
   {
      string fileName = DataPath + i->first;
      cout << Indent << "FileName: " << fileName << endl;
      gdcmHeader* tested = new gdcmHeader( fileName.c_str(), false );
      if( !tested->IsReadable() )
      {
        cerr << Indent << "Image not gdcm compatible:"
             << fileName << endl;
        delete tested;
        return false;
      }

      MapEntryValuesPtr KeyValues = i->second;
      for (MapEntryValues::iterator j  = KeyValues->begin();
                                    j != KeyValues->end();
                                    ++j)
      {
         string key = j->first;

         string groupString  = key.substr( 0, 4 );
         char* groupCharPtr;
         groupCharPtr = new char(groupString.length() + 1);
         strcpy( groupCharPtr, groupString.c_str() ); 

         string groupElement = key.substr( key.find_first_of( "|" ) + 1, 4 );
         char* groupElementPtr;
         groupElementPtr = new char(groupElement.length() + 1);
         strcpy( groupElementPtr, groupElement.c_str() ); 

         uint16_t group   = axtoi( groupCharPtr );
         uint16_t element = axtoi( groupElementPtr );

         string testedValue = tested->GetEntryByNumber(group, element);
         if ( testedValue != j->second )
         {
            cout << Indent << "Uncorrect value for key " << key << endl
                 << Indent << "   read value " << testedValue << endl
                 << Indent << "   reference value " << j->second << endl;
            return false;
         }
      }
      delete tested;
      cout << Indent << "  OK" << endl;
   }
   cout << Indent << endl;
   return true; //???
}

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
   throw ( ParserException )
{
   eatwhite( toSplit );
   string::size_type beginPos = toSplit.find_first_of( '"' );
   string::size_type   endPos = toSplit.find_last_of( '"' );

   // Make sure we have at most to " in toSplit:
   string noQuotes = toSplit.substr( beginPos + 1, endPos - beginPos - 1);
   if ( noQuotes.find_first_of( '"' ) != string::npos )
      throw ParserException( "more than two quote character" );

   // No leading quote means this is not a value:
   if ( beginPos == string::npos )
   {
      return string();
   }

   if ( ( endPos == string::npos ) || ( beginPos == endPos ) )
      throw ParserException( "unmatched \" (quote character)" );

   if ( beginPos != 0 )
   {
      ostringstream error;
      error  << "leading character ["
             << toSplit.substr(beginPos -1, 1)
             << "] before opening \" ";
      throw ParserException( error.str() );
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
      throw ParserException( error.str() );
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
void ReferenceFileParser::FirstPassReferenceFile() throw ( ParserException )
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
            throw ParserException( "Syntax error: EOF reached when in block.");
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
         ostringstream error;
         error << "Syntax error: nested block (open) in reference file"
               << endl
               << ParserException::GetIndent()
               << "   at line " << lineNumber << endl;
         throw ParserException( error.str() );
      }

      // Two occurences of closing blocks on a single line implies nested
      // blocks which is illegal:
      if ( line.find_first_of( "]" ) != line.find_last_of( "]" ) )
      {
         ostringstream error;
         error << "Syntax error: nested block (close) in reference file"
               << endl
               << ParserException::GetIndent()
               << "   at line " << lineNumber << endl;
         throw ParserException( error.str() );
      }

      bool beginBlock ( line.find_first_of("[") != string::npos );
      bool endBlock   ( line.find_last_of("]")  != string::npos );

      // Opening and closing of block on same line:
      if ( beginBlock && endBlock )
      {
         ostringstream error;
         error << "Syntax error: opening and closing on block on same line "
               << lineNumber++ << endl;
         throw ParserException( error.str() );
      }

      // Illegal closing block when block not open:
      if ( !inBlock && endBlock )
      {
         ostringstream error;
         error << "Syntax error: unexpected end of block at line "
               << lineNumber++ << endl;
         throw ParserException( error.str() );
      }
  
      // Uncommented line outside of block is not clean:
      if ( !inBlock && !beginBlock )
      {
         continue;
      }

      if ( inBlock && beginBlock )
      {
         ostringstream error;
         error << "   Syntax error: illegal opening of nested block at line "
               << lineNumber++ << endl;
         throw ParserException( error.str() );
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
}

ReferenceFileParser::ReferenceFileParser()
{
   lineNumber = 1;
   Indent = "      ";
}

bool ReferenceFileParser::Open( string& referenceFileName )
{
   from.open( referenceFileName.c_str(), ios::in );
   if ( !from.is_open() )
   {
      cerr << Indent << "Can't open reference file." << endl;
   }
   
   try
   {
      FirstPassReferenceFile();
      SecondPassReferenceFile();
   }
   catch ( ParserException except )
   {
      except.Print();
      return false;
   }

   from.close();
   return true; //??
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

void ReferenceFileParser::HandleFileName( string& line )
   throw ( ParserException )
{
   if ( line.length() == 0 )
      throw ParserException( "empty line on call of HandleFileName" );

   if ( CurrentFileName.length() != 0 )
      return;

   CurrentFileName = ExtractFirstString(line);
}

void ReferenceFileParser::HandleKey( string& line )
   throw ( ParserException )
{
   if ( CurrentKey.length() != 0 )
      return;

   CurrentKey = ExtractFirstString(line);
   if ( CurrentKey.find_first_of( "|" ) == string::npos )
   {
      ostringstream error;
      error  << "uncorrect key:" << CurrentKey;
      throw ParserException( error.str() );
   }
}

bool ReferenceFileParser::HandleValue( string& line )
   throw ( ParserException )
{
   if ( line.length() == 0 )
      throw ParserException( "empty line in HandleValue" );

   if ( CurrentKey.length() == 0 )
   {
      cout << Indent << "No key present:" << CurrentKey << endl;
      return false;
   }
   
   string newCurrentValue = ExtractValue(line);
   if ( newCurrentValue.length() == 0 )
   {
      ostringstream error;
      error  << "missing value for key:" << CurrentKey;
      throw ParserException( error.str() );
   }

   CurrentValue += newCurrentValue;
   return true;
}

void ReferenceFileParser::ParseRegularLine(string& line)
   throw ( ParserException )
{
   if ( line.length() == 0 )
      return;

   // First thing is to get a filename:
   HandleFileName( line );

   if ( line.length() == 0 )
      return;

   // Second thing is to get a key:
   HandleKey( line );
       
   if ( line.length() == 0 )
      return;

   // Third thing is to get a value:
   if ( ! HandleValue( line ) )
      return;

   if ( CurrentKey.length() && CurrentValue.length() )
   {
      if ( ! AddKeyValuePairToMap( CurrentKey, CurrentValue ) )
         throw ParserException( "adding to map of (key, value) failed" );
      CurrentKey.erase();
      CurrentValue.erase();
   }
}

bool ReferenceFileParser::SecondPassReferenceFile()
   throw ( ParserException )
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
         inBlock = true;
         line.erase( 0, line.find_first_of( "[" ) + 1 );
         eatwhite( line );
         CurrentMapEntryValuesPtr = new MapEntryValues();
      }
      else if ( endBlock )
      {
         line.erase( line.find_last_of( "]" ) );
         eatwhite( line );
         ParseRegularLine( line );
         ProducedMap[CurrentFileName] = CurrentMapEntryValuesPtr;
         inBlock = false;
         CurrentFileName.erase();
      }
   
      // Outside block lines are dropped:
      if ( ! inBlock )
         continue;

      ParseRegularLine( line );
   }
   return true; //??
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

   string referenceDir = GDCM_DATA_ROOT;
   referenceDir       += "/";
   string referenceFilename = referenceDir + "TestAllEntryVerifyReference.txt";

   ReferenceFileParser Parser;
   Parser.Open(referenceFilename);
   Parser.SetDataPath(referenceDir);
   // Parser.Print();
   Parser.Check();
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
