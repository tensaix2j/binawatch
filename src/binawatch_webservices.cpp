
#include "binawatch_shared_data.h"
#include "binawatch_webservices.h"
#include "binawatch_apicaller.h"
#include "binawatch_httpd.h"
#include "binawatch_utils.h"
#include "binawatch_db.h"

struct Binawatch_shared_data* Binawatch_webservices::shared_data = NULL;


//-----------------
int 
Binawatch_webservices::url_router( struct MHD_Connection *connection, const char* url , string &str_response ) {

    if ( strcmp( url , "/allBookTickers.json" ) == 0 ) {
        
        Binawatch_webservices::get_allBookTickers(str_response);

    } else if ( strcmp( url , "/account.json" ) == 0 ) {

        Binawatch_webservices::get_account(str_response);

    } else if ( strcmp( url, "/register.json") == 0 ) {

        string username, password;
        const char* cstr_username = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "username");
        const char* cstr_password = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "password");
        username = cstr_username != NULL ? string(cstr_username ) : "";
        password = cstr_password != NULL ? string(cstr_password ) : "";
        register_account( str_response, username, password );  


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


//---------------
void 
Binawatch_webservices::register_account( string &str_response, string &username , string &password ) 
{
    Binawatch_httpd::write_log("<Binawatch_webservices::register_account> username = %s, password = %s" , username.c_str(), password.c_str() );
    
    Json::Value json_response;
    vector <string> sql_args;

    if ( username != ""  && password != "" ) {

        sql_args.push_back( username );

        Binawatch_db::exec_sql("select * from tbl_users where username = ?", sql_args );
        if ( Binawatch_db::results_set.size() == 0 ) {
            
            Binawatch_httpd::write_log("OK to register. Let's go aheard" );
            
            string hashed_password = sha256( password.c_str() );
            sql_args.push_back( hashed_password );

            int ret = Binawatch_db::exec_sql("insert into tbl_users(username,hashed_password) values( ?, ? )", sql_args );
            if ( ret == 0 ) {
                json_response["status"] = "OK";
                json_response["code"] = 0; 
            } else {
                json_response["status"] = "Some error on our server code. We'll fix it a.s.a.p";
                json_response["code"] = -1; 
            }
        } else {

            Binawatch_httpd::write_log("Username already exist." );
            json_response["status"] = "Username already exist.";
            json_response["code"] = -1; 
        }
    } else {

        json_response["status"] = "Username and Password cannot be empty.";
        json_response["code"] = -1; 
        
    }

    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;
    

}