#include <map>
#include <string>
#include <iostream>

int main()
{
  typedef map<string, char*> dict;
  
  dict current;
	dict::iterator im = current.find("00380010");
  current["00100010"] = "Patient Name";
  current["7fe00010"] = "Pixel Data";
  current["50000010"] = "Number of points";
  current["00380010"] = "Admission ID";

	cout << "Traversal of dictionary (note the proper ordering on key)." << endl;
	for ( dict::iterator im = current.begin(); im != current.end(); ++im )
		cout << "   \"" << im->first << "\" = " << im->second << endl;
 	cout << "End of dictionary." << endl;

 	cout << "Find request on key 00380010" << endl;
	im = current.find("00380010");
	cout << "   \"" << im->first << "\" = " << im->second << endl;
}
