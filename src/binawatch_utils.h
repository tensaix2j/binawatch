
#include <string>
#include <sstream>
#include <vector>
#include <string.h>

using namespace std;

void split_string( string &s, char delim, vector <string> &result);
bool replace_string( string& str, const char *from, const char *to);

inline bool file_exists (const std::string& name) {
 	 return ( access( name.c_str(), F_OK ) != -1 );
}
