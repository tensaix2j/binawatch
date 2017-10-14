





#include "binawatch_httpd.h"
#include "binawatch_webservices.h"
#include "binawatch_shared_data.h"
#include "binawatch_db.h"
#include "binawatch_logger.h"


//----------------------------------
int main( int argc , char *argv[] ) {

	Binawatch_logger::enable_logfile(0);

	struct Binawatch_shared_data shared_data; 
	Binawatch_db::init();
   	Binawatch_webservices::shared_data = &shared_data;
   	Binawatch_httpd::init( 10001 );
   	
   	while (1) {
   		if ( Binawatch_httpd::has_pending_request() ) {
			Binawatch_httpd::process_request_queue();
		}
		usleep(1000);
	}
   	return 0;
}
