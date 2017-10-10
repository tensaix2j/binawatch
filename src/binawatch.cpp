





#include "binawatch_httpd.h"
#include "binawatch_webservices.h"
#include "binawatch_shared_data.h"



//----------------------------------
int main( int argc , char *argv[] ) {

	struct Binawatch_shared_data shared_data; 

   	Binawatch_webservices::shared_data = &shared_data;
   	Binawatch_httpd::init( 10001 );
   	

	while (1) {

		if ( Binawatch_httpd::has_pending_request() ) {
			
			Binawatch_httpd::process_request_queue();
		
		} else {
			usleep(1000);
		}
	}


   	return 0;
}
