#include "gdcmUtil.h"

int TestDicomString(int , char* [])
{
  int i;
  const char *s = "\0\0";
  std::string a(s,s+2); // will copy 2 '\0' 
  assert( a.size() == 2 );
  for(i=0;i<2;i++)
  {
    assert( a.c_str()[i] == '\0' );
    assert( a.data()[i] == '\0' );
  }
  assert( a.c_str()[2] == '\0' );

/*
std::string zeros(x, 0);
char s1[] = "\0";
char s2[] = "\0\0";
char s3[] = "\0\0\0";
char s4[] = "\0abc";*/

  return 0;
}
