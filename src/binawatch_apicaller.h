
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <exception>


#include <curl/curl.h>
#include <json/json.h>


#include "binawatch_shared_data.h"

#define BINANCE_HOST "https://www.binance.com"

using namespace std;

class Binawatch_apicaller {

	public:

		static struct Binawatch_shared_data *shared_data;
		static void init();
		static void write_log( const char *fmt, ... );
		static void curl_api( string &url, string &result_json );
		static size_t curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) ;
		static void get_allBookTickers();
		static void get_account();

};


