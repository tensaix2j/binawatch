


#include <sys/time.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <fnmatch.h>



#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

#include <microhttpd.h>


using namespace std;

class Binawatch_httpd {

	static map < string, int > sessions_registered;

	public:
		static struct MHD_Daemon *daemon;
		
		static void write_log( const char *fmt, ... );

		
		static int answer_to_connection( 
			void *cls, 
	        struct MHD_Connection *connection,
	        const char *url, 
	        const char *method,
	        const char *version, 
	        const char *upload_data,
	        size_t *upload_data_size, 
	        void **con_cls  
	    ); 
		static int log_http_header ( void *cls, enum MHD_ValueKind kind, const char *key, const char *value );

		static int response_with_errormsg( struct MHD_Connection *connection, MHD_Response *response, int errorcode, const char *error_msg  );
		static int response_with_static_resource( 
	        struct MHD_Connection *connection, 
	        MHD_Response *response,
	        char* mime_type,
	        const char *url,
	        struct stat *sbuf
	    ) ;

		static int response_with_web_service( 
		    struct MHD_Connection *connection, 
		    MHD_Response *response,
		    const char *url
		); 

		static string get_session( struct MHD_Connection *connection );
		static string generate_new_session();
		static void expiring_sessions();

		static int init( int port );
		static int stop();

};




