

#include <string.h>
#include <fnmatch.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

#include <json/json.h>

using namespace std;

class Binawatch_webservices {

	public:
		static struct Binawatch_shared_data *shared_data;
		static void url_router( const char* url , string &str_response );
		static void get_allBookTickers( string &str_response );
		static void get_account( string &str_response );
};


