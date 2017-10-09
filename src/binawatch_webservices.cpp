
#include "binawatch_shared_data.h"
#include "binawatch_webservices.h"


struct Binawatch_shared_data* Binawatch_webservices::shared_data = NULL;


//-----------------
void 
Binawatch_webservices::url_router( const char* url , string &str_response ) {

    if ( strcmp( url , "/allBookTickers.json" ) == 0 ) {
        
        Binawatch_webservices::get_allBookTickers(str_response);

    } else if ( strcmp( url , "/account.json" ) == 0 ) {

        Binawatch_webservices::get_account(str_response);


    } else {

        Json::Value json_response;
        json_response["status"] = "Not found";
        json_response["code"] = -1; 
        Json::FastWriter fastWriter;
        str_response = fastWriter.write(json_response) ;
    }

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

    Json::Value account_obj;
    account_obj["username"] = "hello world";

    Json::FastWriter fastWriter;
    str_response = fastWriter.write(account_obj) ;
    
}
