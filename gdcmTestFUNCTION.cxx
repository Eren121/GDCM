// Minimal test for existence of __FUNCTION__ pseudo-macro
#include <string.h>

int TestFUNCTION()
{
  const char *f = __FUNCTION__;
  int r = strcmp( "TestFUNCTION", f);
  return r;
}
 
int main()
{
  return TestFUNCTION();
}

