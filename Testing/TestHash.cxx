#include <map>
#include <string>
#include <iostream>

int main()
{
  typedef map<string, char*> dict;
  
  dict tb1;
  dict::iterator im = tb1.find("00380010");
  tb1["00100010"] = "Patient Name";
  tb1["7fe00010"] = "Pixel Data";
  tb1["50000010"] = "Number of points";
  tb1["00380010"] = "Admission ID";

	cout << "Traversal of dictionary (note the proper ordering on key)." << endl;
	for ( dict::iterator im = tb1.begin(); im != tb1.end(); ++im )
		cout << "   \"" << im->first << "\" = " << im->second << endl;
 	cout << "End of dictionary." << endl;

 	cout << "Find request on key 00380010" << endl;
	im = tb1.find("00380010");
	cout << "   \"" << im->first << "\" = " << im->second << endl;
}
