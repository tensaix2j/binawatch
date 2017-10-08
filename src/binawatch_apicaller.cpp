
#include "binawatch_apicaller.h"



struct Binawatch_shared_data* Binawatch_apicaller::shared_data = NULL;


//------------------------
void 
Binawatch_apicaller::write_log( const char *fmt, ... ) 
{
    va_list arg;
    
    char new_fmt[1024];
    
   	struct timeval tv;
    gettimeofday(&tv, NULL); 
    time_t t = tv.tv_sec;
    struct tm * now = localtime( &t );
    
    sprintf( new_fmt , "%04d-%02d-%02d %02d:%02d:%02d %06ld :%s\n" , now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec , tv.tv_usec , fmt );

    va_start (arg, fmt);
    vfprintf ( stdout, new_fmt, arg);
    va_end (arg);

    fflush(stdout);
}





//-----------------
// Curl's callback
size_t 
Binawatch_apicaller::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	write_log( "<Binawatch_apicaller::curl_cb> " ) ;

	size_t newLength = size*nmemb;
    size_t oldLength = buffer->size();
    try
    {
        buffer->resize(oldLength + newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }

    std::copy((char*)content,(char*)content + newLength,buffer->begin()+oldLength);

    write_log( "<Binawatch_apicaller::curl_cb> done" ) ;

    return size*nmemb;
}






//--------------------
// Do the curl
void 
Binawatch_apicaller::curl_api( string &url, string &result_json) 
{
	write_log( "<Binawatch_apicaller::curl_api>" ) ;

	CURL *curl;
	CURLcode res;
	
	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	
	if( curl ) { 

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Binawatch_apicaller::curl_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result_json );

		res = curl_easy_perform(curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			write_log( "<Binawatch_apicaller::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		} 	
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	write_log( "<Binawatch_apicaller::curl_api> done" ) ;

}





//--------------------
// Get market data
void 
Binawatch_apicaller::get_allBookTickers() 
{	

	write_log( "<Binawatch_apicaller::get_allBookTickers>" ) ;

	string result_json;
	string url = "https://www.binance.com/api/v1/ticker/allBookTickers";
	curl_api( url, result_json) ;


	if ( result_json.size() > 0 ) {
		
		try {
			Json::Reader reader;
	    	Json::Value obj;
	    	
	    	reader.parse( result_json , obj);
	    	
	    	string symbol;
		    double bidPrice, askPrice, bidQty, askQty;

	    	for (int i = 0; i < obj.size(); i++){

	    		symbol 		= obj[i]["symbol"].asString();
	    		bidPrice    = atof( obj[i]["bidPrice"].asString().c_str() );
	        	bidQty      = atof( obj[i]["bidQty"].asString().c_str() );
	        	askPrice    = atof ( obj[i]["askPrice"].asString().c_str() );
	        	askQty      = atof( obj[i]["askQty"].asString().c_str() );
	        	
	        	
        		if ( shared_data ) {

        			shared_data->bidPrice[symbol] 	= bidPrice;
					shared_data->bidQty[symbol] 	= bidQty;
		 			shared_data->askPrice[symbol] 	= askPrice;
		 			shared_data->askQty[symbol] 	= askQty;
		 		}
	 		}
	    } catch ( exception &e ) {
		 	write_log( "<Binawatch_apicaller::get_allBookTickers> Error ! %s", e.what() ); 
		}   
		write_log( "<Binawatch_apicaller::get_allBookTickers> Done." ) ;
	
	} else {
		write_log( "<Binawatch_apicaller::get_allBookTickers> Failed to get anything." ) ;
	}


}




//------------------------
void 
Binawatch_apicaller::init() 
{
	write_log("<Binawatch_apicaller::init>");

	while (1) {

		write_log("<Binawatch_apicaller::init> Calling API...");
		get_allBookTickers();
		sleep(60);
	}
}

