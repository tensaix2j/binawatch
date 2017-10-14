
#include "binawatch_apicaller.h"
#include "binawatch_shared_data.h"
#include "binawatch_utils.h"
#include "binawatch_db.h"
#include "binawatch_logger.h"




struct Binawatch_shared_data* Binawatch_apicaller::shared_data = NULL;



//--------------------
// Get Best price/qty on the order book for all symbols.

void 
Binawatch_apicaller::get_allBookTickers() 
{	

	Binawatch_logger::write_log( "<Binawatch_apicaller::get_allBookTickers>" ) ;

	string url(BINANCE_HOST);  
	url += "/api/v1/ticker/allBookTickers";

	string result_json;
	
	curl_api( url, result_json ) ;


	if ( result_json.size() > 0 ) {
		
		try {
			Json::Reader reader;
	    		Json::Value obj;
	    	
	    		reader.parse( result_json , obj);
	    	
	    		string symbol, bidPrice, askPrice, bidQty, askQty;
	    	
	    		for (int i = 0; i < obj.size(); i++){

	    			symbol		= obj[i]["symbol"].asString();
	    			bidPrice	= obj[i]["bidPrice"].asString();
	        		bidQty		= obj[i]["bidQty"].asString();
	        		askPrice	= obj[i]["askPrice"].asString();
	        		askQty		= obj[i]["askQty"].asString();
	        	
	        	
	        		if ( shared_data ) {

	        			shared_data->bidPrice[symbol] 	= bidPrice;
					shared_data->bidQty[symbol] 	= bidQty;
			 		shared_data->askPrice[symbol] 	= askPrice;
			 		shared_data->askQty[symbol] 	= askQty;
			 		shared_data->last_query_time    = get_current_epoch();
			 	}
		 	}

		} catch ( exception &e ) {
		 	Binawatch_logger::write_log( "<Binawatch_apicaller::get_allBookTickers> Error ! %s", e.what() ); 
		}   
		Binawatch_logger::write_log( "<Binawatch_apicaller::get_allBookTickers> Done." ) ;
	
	} else {
		Binawatch_logger::write_log( "<Binawatch_apicaller::get_allBookTickers> Failed to get anything." ) ;
	}
}









//--------------------
// Get current account information.
/*
Parameters:
Name	Type	Mandatory	Description
recvWindow	LONG	NO	
timestamp	LONG	YES
*/


void 
Binawatch_apicaller::get_account( const char* api_key, const char *secret_key , string &str_result ) 
{	

	Binawatch_logger::write_log( "<Binawatch_apicaller::get_account>" ) ;

	string url(BINANCE_HOST);
	url += "/api/v3/account?";

	string querystring("timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key, querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	Binawatch_logger::write_log( "<Binawatch_apicaller::get_account> url = |%s|" , url.c_str() ) ;
	
	curl_api_with_header( url, str_result , extra_http_header ) ;

}












//-----------------
// Curl's callback
size_t 
Binawatch_apicaller::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	Binawatch_logger::write_log( "<Binawatch_apicaller::curl_cb> " ) ;

	size_t newLength = size*nmemb;
	size_t oldLength = buffer->size();
	try {
		buffer->resize(oldLength + newLength);
	
	} catch(std::bad_alloc &e) {
		//handle memory problem
		return 0;
	}

	std::copy((char*)content,(char*)content + newLength,buffer->begin()+oldLength);
	Binawatch_logger::write_log( "<Binawatch_apicaller::curl_cb> done" ) ;
	return size*nmemb;
}






//--------------------------------------------------
void 
Binawatch_apicaller::curl_api( string &url, string &result_json ) {
	vector <string> v;
	curl_api_with_header( url , result_json , v );	
} 



//--------------------
// Do the curl
void 
Binawatch_apicaller::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header ) 
{
	Binawatch_logger::write_log( "<Binawatch_apicaller::curl_api>" ) ;

	CURL *curl;
	CURLcode res;
	
	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	
	if( curl ) { 

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Binawatch_apicaller::curl_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
 			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
 		}

		res = curl_easy_perform(curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			Binawatch_logger::write_log( "<Binawatch_apicaller::curl_api> curl_easy_perform() failed: %s" , curl_easy_strerror(res) ) ;
		} 	
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	Binawatch_logger::write_log( "<Binawatch_apicaller::curl_api> done" ) ;

}





//------------------------
void 
Binawatch_apicaller::init() 
{
	Binawatch_logger::write_log("<Binawatch_apicaller::init>");

	
}

