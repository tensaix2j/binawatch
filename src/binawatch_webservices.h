

#include <string.h>
#include <fnmatch.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

#include <json/json.h>


#include <microhttpd.h>


using namespace std;

class Binawatch_webservices {

	public:
		static struct Binawatch_shared_data *shared_data;
		static int url_router( struct MHD_Connection *connection, string &str_session_id, const char* url , string &str_response );
		static void get_allBookTickers( string &str_response );
		static void get_account( string &str_response );
		static void register_account( string &str_response, string &username , string &password ) ;
		static void login_account(string &str_response, string &str_session_id, string &username , string &password );

};


