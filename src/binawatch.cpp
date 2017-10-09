





#include "binawatch_httpd.h"
#include "binawatch_webservices.h"
#include "binawatch_apicaller.h"
#include "binawatch_shared_data.h"



//----------------------------------
int main( int argc , char *argv[] ) {

	struct Binawatch_shared_data shared_data; 

   	Binawatch_webservices::shared_data = &shared_data;
   	Binawatch_httpd::init( 10001 );
   	Binawatch_apicaller::shared_data = &shared_data;
	
	while (1) {

		Binawatch_apicaller::get_allBookTickers();
		Binawatch_httpd::expiring_sessions();
		sleep(60);
	}


   	return 0;
}
