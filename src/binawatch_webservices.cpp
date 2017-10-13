
#include "binawatch_shared_data.h"
#include "binawatch_webservices.h"
#include "binawatch_apicaller.h"
#include "binawatch_httpd.h"
#include "binawatch_utils.h"
#include "binawatch_db.h"

struct Binawatch_shared_data* Binawatch_webservices::shared_data = NULL;















//-----------------
int 
Binawatch_webservices::url_router( struct MHD_Connection *connection, string &str_session_id, string &url , string &str_response ) {

    if ( url == "/allBookTickers.json"  ) {
        
        get_allBookTickers(str_response);

    } else if ( url == "/account.json"  ) {

        get_account(str_response);


    } else if ( url == "/register.json" ) {

            
        string username, password;
        const char* cstr_username = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "username");
        const char* cstr_password = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "password");
        username = cstr_username? string(cstr_username ) : "";
        password = cstr_password? string(cstr_password ) : "";
        register_account( str_response, username, password );  
    


    } else if ( url == "/login.json"  ) {

        string username, password;
        const char* cstr_username = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "username");
        const char* cstr_password = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "password");
        username = cstr_username? string(cstr_username ) : "";
        password = cstr_password? string(cstr_password ) : "";
        
        login_account(str_response, str_session_id,  username, password );
        

    } else if ( url == "/logout.json" ) {
    
        logout_account( str_response, str_session_id );


    } else if ( url == "/save_apikey.json" ) {

        string apikey, secretkey;
        const char* cstr_apikey     = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "apikey");
        const char* cstr_secretkey  = MHD_lookup_connection_value( connection, MHD_GET_ARGUMENT_KIND, "secretkey");
        apikey      = cstr_apikey? string(cstr_apikey ) : "";
        secretkey   = cstr_secretkey? string(cstr_secretkey ) : "";
        
        save_apikey( str_response, str_session_id, apikey, secretkey ); 


     } else if ( url == "/get_apikey.json" ) {
        
        get_apikey( str_response, str_session_id ); 
        

    } else {

        Json::Value json_response;
        json_response["statusmsg"] = "Not found";
        json_response["statuscode"] = -1; 
        Json::FastWriter fastWriter;
        str_response = fastWriter.write(json_response) ;
    }

    return 0;
}   















//---------------------------
void
Binawatch_webservices::get_allBookTickers( string &str_response) {

    Binawatch_httpd::write_log("<Binawatch_webservices::get_allBookTickers>");

	Json::Value json_response(Json::arrayValue);
        
    // This data can be shared across all users.
    if ( get_current_epoch() - shared_data->last_query_time  > 5 ) {
        Binawatch_apicaller::get_allBookTickers();
    }

    
    map <string,string>::iterator it;
    for ( it = shared_data->bidPrice.begin(); it != shared_data->bidPrice.end(); ++it) {
            
        Json::Value symbol;
        symbol["s"]    = it->first ;
        symbol["b"]  = it->second ;
        symbol["bq"]    = shared_data->bidQty[it->first] ;
        symbol["a"]  = shared_data->askPrice[it->first] ;
        symbol["aq"]    = shared_data->askQty[it->first] ;
        json_response.append( symbol );

    }
   
    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;
    
    Binawatch_httpd::write_log("<Binawatch_webservices::get_allBookTickers> Ready to response. |%s|... ", str_response.substr(0,100).c_str() );
    
    

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
    Binawatch_httpd::write_log("<Binawatch_webservices::register_account> username = %s, password = *** " , username.c_str() );
    
    Json::Value json_response;
    vector <string> sql_args;

    if ( username != ""  && password != "" ) {

        sql_args.push_back( username );

        Binawatch_db::exec_sql("select * from tbl_users where username = ?", sql_args );
        if ( Binawatch_db::results_set.size() == 0 ) {
            
            
            string hashed_password = sha256( password.c_str() );
            sql_args.push_back( hashed_password );

            int ret = Binawatch_db::exec_sql("insert into tbl_users(username,hashed_password) values( ?, ? )", sql_args );
            if ( ret == 0 ) {
                
                json_response["statusmsg"] = "OK";
                json_response["statuscode"] = 0; 
                Binawatch_httpd::write_log("Register OK!" );

            } else {
                json_response["statusmsg"] = "Some error on our server code. We'll fix it a.s.a.p";
                json_response["statuscode"] = -1; 
            }
        } else {

            Binawatch_httpd::write_log("Username already exist." );
            json_response["statusmsg"] = "Username already exist.";
            json_response["statuscode"] = -2; 
        }
    } else {

        json_response["statusmsg"] = "Username and Password cannot be empty.";
        json_response["statuscode"] = -1; 
        
    }

    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;
    

}

















//---------------
void 
Binawatch_webservices::login_account( string &str_response, string &str_session_id,  string &username , string &password ) 
{
    Binawatch_httpd::write_log("<Binawatch_webservices::login_account> username = %s, password = %s" , username.c_str(), password.c_str() );
    
    Json::Value json_response;
    vector <string> sql_args;

    if ( username != ""  && password != "" ) {

        sql_args.push_back( username );

        Binawatch_db::exec_sql("select username,hashed_password from tbl_users where username = ?", sql_args );
        if ( Binawatch_db::results_set.size() == 0 ) {
            
            Binawatch_httpd::write_log("User does not exist." );
            json_response["statusmsg"] = "User does not exist.";
            json_response["statuscode"] = -2; 

        } else {

            string hashed_password = sha256( password.c_str() );
            if ( hashed_password == Binawatch_db::results_set[0][1] ) {
                // OK
                json_response["statusmsg"] = "OK";
                json_response["statuscode"] = 0; 
                Binawatch_httpd::write_log("Login OK!" );
                Binawatch_httpd::add_login_user( str_session_id, username );
                
            } else {
                json_response["statusmsg"] = "Authentication Failed!";
                json_response["statuscode"] = -3; 
            }

        }
    } else {

        json_response["statusmsg"] = "Username and Password cannot be empty.";
        json_response["statuscode"] = -1; 
        
    }

    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;

}






//---------------
void 
Binawatch_webservices::logout_account( string &str_response, string &str_session_id ) 
{
    Binawatch_httpd::write_log("<Binawatch_webservices::logout_account>" );
    
    Json::Value json_response;
    
    Binawatch_httpd::remove_login_user( str_session_id );
    json_response["statusmsg"] = "OK";
    json_response["statuscode"] = 0; 

    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;

}





//-------------
void 
Binawatch_webservices::save_apikey( string &str_response, string &str_session_id, string &apikey, string &secretkey ) {

    Binawatch_httpd::write_log("<Binawatch_webservices::save_apikey>");
    
    Json::Value json_response;
    vector <string> sql_args;

    struct login_user *user = Binawatch_httpd::get_session_user( str_session_id );

    if ( user != NULL ) {
        
        user->apikey = apikey; 
        user->secretkey = secretkey;
        sql_args.push_back( apikey );
        sql_args.push_back( secretkey );
        sql_args.push_back( user->username );
        
        int ret = Binawatch_db::exec_sql("update tbl_users set api_key = ?, secret_key = ? where username = ?", sql_args );
        if ( ret == 0 ) {
            json_response["statusmsg"] = "OK";
            json_response["statuscode"] = 0; 
        } else {
            json_response["statusmsg"] = "Error saving api keys";
            json_response["statuscode"] = -1; 
        }
    } else {
        json_response["statusmsg"] = "User Operation FAILED";
        json_response["statuscode"] = -5; 
    }


    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;

}









//-------------
void 
Binawatch_webservices::get_apikey( string &str_response, string &str_session_id ) {

    Binawatch_httpd::write_log("<Binawatch_webservices::get_apikey>");
    
    Json::Value json_response;
    vector <string> sql_args;

    struct login_user *user = Binawatch_httpd::get_session_user( str_session_id );

    if ( user != NULL ) {
        
        json_response["statusmsg"] = "OK";
        json_response["statuscode"] = 0; 
        json_response["apikey"]    = user->apikey; 
        json_response["secretkey"] = user->secretkey; 
        
        
    } else {
        json_response["statusmsg"] = "User Operation FAILED";
        json_response["statuscode"] = -5; 
    }


    Json::FastWriter fastWriter;
    str_response = fastWriter.write(json_response) ;

}