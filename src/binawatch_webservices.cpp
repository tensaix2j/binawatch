
#include "binawatch_shared_data.h"
#include "binawatch_webservices.h"
#include "binawatch_apicaller.h"
#include "binawatch_httpd.h"
#include "binawatch_utils.h"

struct Binawatch_shared_data* Binawatch_webservices::shared_data = NULL;


//-----------------
int 
Binawatch_webservices::url_router( struct MHD_Connection *connection, const char* url , string &str_response ) {

    if ( strcmp( url , "/allBookTickers.json" ) == 0 ) {
        
        Binawatch_webservices::get_allBookTickers(str_response);

    } else if ( strcmp( url , "/account.json" ) == 0 ) {

        Binawatch_webservices::get_account(str_response);

    } else if ( strcmp( url, "/register.json") == 0 ) {

        const char* username = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "username");
        Binawatch_httpd::write_log("username = %s" , username);
        
        const char* password = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "password");
        Binawatch_httpd::write_log("password = %s" , password);


    } else {

        Json::Value json_response;
        json_response["status"] = "Not found";
        json_response["code"] = -1; 
        Json::FastWriter fastWriter;
        str_response = fastWriter.write(json_response) ;
    }

    return 0;
}   


//---------------------------
void
Binawatch_webservices::get_allBookTickers( string &str_response) {

	Json::Value tickers_arr(Json::arrayValue);
        
    map <string,double>::iterator it;
    for ( it = shared_data->bidPrice.begin(); it != shared_data->bidPrice.end(); ++it) {
            
        Json::Value symbol;
        symbol["symbol"]    = it->first ;
        symbol["bidPrice"]  = it->second ;
        symbol["bidQty"]    = shared_data->bidQty[it->first] ;
        symbol["askPrice"]  = shared_data->askPrice[it->first] ;
        symbol["askQty"]    = shared_data->askQty[it->first] ;
        tickers_arr.append( symbol );

    }
   
    Json::FastWriter fastWriter;
    str_response = fastWriter.write(tickers_arr) ;
    
}




//---------------------------
void
Binawatch_webservices::get_account( string &str_response ) {

    Binawatch_apicaller::get_account(
        "w5mAgtyqd7NWoQVle0UbA56lb0ZpYkBtCUBcuNlgGa5Jn7wznU22ea8IWkWIxKxu",
        "NIteeFjxCrbYWIEXOa6FTSIQrWeJOoXG1fUaU7sgT2SGjlp2iFoyXUr7hbmQouBp");
        
}
