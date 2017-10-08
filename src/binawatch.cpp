

#include <pthread.h>
#include "binawatch_httpd.h"
#include "binawatch_apicaller.h"
#include "binawatch_shared_data.h"


//----------------------
void *api_caller( void *shared_data ) 
{
	Binawatch_apicaller::shared_data = (struct Binawatch_shared_data *)shared_data;
	Binawatch_apicaller::init();
	pthread_exit(NULL);
}




//----------------------
void *httpd_event_loop( void *shared_data ) 
{	
	Binawatch_httpd::shared_data = (struct Binawatch_shared_data *)shared_data;
	Binawatch_httpd::init( 10001 );
    pthread_exit(NULL);
}



//----------------------------------
int main( int argc , char *argv[] ) {

	// Need 2 threads 1 for httpd, 1 for api_caller so they don't block one another..
	//pthread_t api_caller_thread;
	//pthread_t httpd_thread;

	//struct Binawatch_shared_data *shared_data = (struct Binawatch_shared_data * ) malloc( sizeof( struct Binawatch_shared_data ) );

	//int rc;
	
	//rc = pthread_create( &api_caller_thread, NULL, api_caller , (void *)shared_data);
    //if (rc) {
    //	cout << "Error:unable to create thread for api_caller : " << rc << endl;
	//	exit(-1);
	//}
	
	/*
	rc = pthread_create( &httpd_thread, NULL, httpd_event_loop , (void *)&shared_data) ;
	if (rc) {
    	cout << "Error:unable to create thread for httpd : " << rc << endl;
		exit(-1);
	}
  	*/
  	// This replaces return for proper thread cleanup. 	
   	//pthread_exit(NULL);
   	

   	struct Binawatch_shared_data shared_data; 
   	

   	Binawatch_httpd::shared_data = &shared_data;
   	Binawatch_httpd::init( 10001 );
   	Binawatch_apicaller::shared_data = &shared_data;
	Binawatch_apicaller::init();
	


   	return 0;
}
