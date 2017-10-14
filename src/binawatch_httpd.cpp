

#include "binawatch_httpd.h"
#include "binawatch_utils.h"
#include "binawatch_apicaller.h"
#include "binawatch_webservices.h"
#include "binawatch_db.h"


struct MHD_Daemon * Binawatch_httpd::daemon = NULL;
map    < string, time_t > Binawatch_httpd::sessions_registered;
vector < struct request > Binawatch_httpd::request_queue;
map    < string, struct login_user >   Binawatch_httpd::sessions_login_users;


//-------------------------------------------------
int
Binawatch_httpd::log_http_header (
        void *cls, 
        enum MHD_ValueKind kind, 
        const char *key,
        const char *value)
{
    write_log ("<Binawatch_httpd::log_http_header> %s: %s", key, value);
    return MHD_YES;
}





//---------------
string 
Binawatch_httpd::generate_new_session() 
{

    char bytearr_session_id[32];
    ifstream ifs("/dev/urandom");
    ifs.read( bytearr_session_id, 32);
    ifs.close();
    
    string str_session_id = b2a_hex( bytearr_session_id , 32 );
    
    write_log("<Binawatch_httpd::generate_new_session> Registering new session %s", str_session_id.c_str() );
    sessions_registered[ str_session_id ] = get_current_epoch() + 3600;

    return str_session_id;
}




//------------------------------------------

string
Binawatch_httpd::get_session ( struct MHD_Connection *connection ) 
{

    write_log("<Binawatch_httpd::get_session>");

    const char *cookie;
    cookie = MHD_lookup_connection_value (connection, MHD_COOKIE_KIND, "session");
    string str_session_id;

    if ( cookie != NULL ) {

        str_session_id = string(cookie);

        if ( sessions_registered.find( str_session_id ) == sessions_registered.end() ) { 
            
            write_log("<Binawatch_httpd::get_session> Session %s expired on server end, get new one", str_session_id.c_str() );
            str_session_id = generate_new_session();

        } else {

            write_log("<Binawatch_httpd::get_session> Session %s found !", str_session_id.c_str() );

            // Renew the session expiry time..
            sessions_registered[ str_session_id ] = get_current_epoch() + 3600;
        }
    } else {
        str_session_id = generate_new_session();
    }
    return str_session_id;

}



//------------------
void 
Binawatch_httpd::expiring_sessions()  
{
    map <string,time_t>::iterator it;
    for ( it = sessions_registered.begin(); it != sessions_registered.end(); ++it) {
            
        string str_session_id       = it->first ;
        time_t session_expiry        = it->second ;

        // sessions to expire in 1 hour
        if ( get_current_epoch() >= session_expiry  ) {
            
            write_log("Session %s expired.", str_session_id.c_str() );
            sessions_registered.erase( it );

        }
    }

}



//-------------
void 
Binawatch_httpd::add_login_user( string &str_session_id, string &username ) 
{       
    write_log("Binawatch_httpd::add_login_user : %s", username.c_str() );
        
    Binawatch_db::results_set.clear();
    vector <string> sql_args;
    sql_args.push_back( username );

    Binawatch_db::exec_sql("select username, api_key, secret_key from tbl_users where username = ?", sql_args );
        
    if ( Binawatch_db::results_set.size() > 0 ) {    
        
        struct login_user new_user;
        
        new_user.username    = username;
        new_user.apikey      = Binawatch_db::results_set[0][1];
        new_user.secretkey   = Binawatch_db::results_set[0][2];
        new_user.expiry_time = get_current_epoch() + 3600;

        write_log("Binawatch_httpd::add_login_user: %s OK", username.c_str() );
        sessions_login_users[ str_session_id ] = new_user;
    }

}


//-----------------
void 
Binawatch_httpd::remove_login_user( string &str_session_id ) 
{       
    write_log("Binawatch_httpd::remove_login_user : %s", str_session_id.c_str() );
    sessions_login_users.erase( str_session_id );
    
}



//--------------
struct login_user *
Binawatch_httpd::get_session_user( string &str_session_id ) {

    if ( sessions_login_users.find( str_session_id ) != sessions_login_users.end() ) {

        return &sessions_login_users[ str_session_id ];
    } else {
        return NULL;
    }
}







//------------------------------------------
int 
Binawatch_httpd::response_with_errormsg( 

    struct MHD_Connection *connection, 
    MHD_Response *response, 
    int errorcode, 
    const char *error_msg ) 
{

    char errorstr[1024];
    sprintf( errorstr , "<html><body>%s</body></html>", error_msg ) ;
    
    write_log("<Binawatch_httpd::response_with_errormsg> %s", error_msg );
    
    response = MHD_create_response_from_buffer ( strlen (errorstr), (void *) errorstr, MHD_RESPMEM_PERSISTENT );

    if ( response != NULL ) {
        int ret = MHD_queue_response ( connection, errorcode , response);
        MHD_destroy_response (response);
        return ret;
    } else {
        return MHD_NO;
    }
}








//---------------------------------
int
Binawatch_httpd::response_with_static_resource( 

        struct MHD_Connection *connection, 
        MHD_Response *response,
        char* mime_type,
        string &url
    ) 
{

    int fd;
    int ret;
    struct stat sbuf;

    string full_path;
    full_path.append( "../web");
    full_path.append( url );
    
    if ( (-1 == (fd = open ( full_path.c_str() , O_RDONLY))) || (0 != fstat (fd, &sbuf)) ) {
        if (fd != -1) {
            (void) close (fd);
        }
        ret = response_with_errormsg( connection,  response, 404 , "File Not Found" );
    }


    write_log("<Binawatch_httpd::answer_to_connection> Ready to serve %s", full_path.c_str() );


    response = MHD_create_response_from_fd_at_offset64 (sbuf.st_size, fd, 0);
    MHD_add_response_header (response, "Content-Type", mime_type);
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);

    return ret;
}






//---------------------------------
int
Binawatch_httpd::response_with_web_service( 

        struct MHD_Connection *connection, 
        MHD_Response *response,
        char* mime_type,
        string &url
    ) 
{

    int ret;
    string str_response;
    string str_session_id = get_session( connection );
    
    if ( fnmatch("/*.html", url.c_str(), FNM_LEADING_DIR ) == 0 ) {
        
        ret = html_routing( connection, response, mime_type, url , str_session_id );

    } else {
        
        Binawatch_webservices::url_router( connection, str_session_id,  url , str_response);
        
        // Response now...
        response = MHD_create_response_from_buffer ( str_response.size() , (void *)( str_response.c_str() ) , MHD_RESPMEM_MUST_COPY);
        // set response header session ID
        
        string cookie_kv = "session=";
        cookie_kv.append( str_session_id );
        cookie_kv.append( ";");

        MHD_add_response_header (response, "Content-Type", mime_type);
        MHD_add_response_header (response, MHD_HTTP_HEADER_SET_COOKIE,  cookie_kv.c_str() );

            
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);
    
    }

        
    return ret;
}













//----------------
int 
Binawatch_httpd::html_routing( 

        struct MHD_Connection *connection, 
        MHD_Response *response,
        char* mime_type,
        string &url, 
        string &str_session_id 
    ) 
{
    int ret;

    if ( url == "/login.html" || url == "/register.html" ) {
            
        struct login_user* user = get_session_user(str_session_id);
        if ( user == NULL ) {
            ret = response_with_static_resource( connection, response, mime_type , url );    
        } else {
            string redirect_url = INDEX_PAGE;
            ret = response_with_static_resource( connection, response, mime_type , redirect_url ); 
        }

    } else if ( url == INDEX_PAGE || url == "/apikey.html" ) {

        struct login_user* user = get_session_user(str_session_id);
        if ( user == NULL ) {
            string redirect_url = "/login.html";
            ret = response_with_static_resource( connection, response, mime_type , redirect_url ); 
        } else {
            write_log("<Binawatch_httpd::html_routing> %p : %s", user, user->username.c_str() );
            ret = response_with_static_resource( connection, response, mime_type , url );    
        }

    } else {
        ret = response_with_static_resource( connection, response, mime_type , url );
    }
    return ret;
}





//-------------------
int 
Binawatch_httpd::answer_to_connection( 

        void *cls, 
        struct MHD_Connection *connection,
        const char *url, 
        const char *method,
        const char *version, 
        const char *upload_data,
        size_t *upload_data_size, 
        void **con_cls 
    ) 
{



    write_log("<Binawatch_httpd::answer_to_connection> New %s request for %s \n", method, url );
    
    struct MHD_Response *response;
    int ret;
    bool is_static_resource = 0;
    bool is_web_service     = 0;
    char mime_type[128] = "text/plain";
    
        

    // Redirection
    string use_url = string(url);
    if ( use_url == "/" ) {
        use_url = INDEX_PAGE;
        write_log("<Binawatch_httpd::answer_to_connection> Redirect New %s request to %s \n", method, use_url.c_str() );
    
    }
        
    

    if ( fnmatch( "/*.css", use_url.c_str(), FNM_LEADING_DIR ) == 0 ) {
    
        sprintf( mime_type, "text/css" );
        is_static_resource = 1;
    
    } else if ( fnmatch("/*.html", use_url.c_str(), FNM_LEADING_DIR ) == 0 ) {
        
        sprintf( mime_type, "text/html" );
        is_web_service = 1;
        
    } else if ( fnmatch("/*.js", use_url.c_str() , FNM_LEADING_DIR ) == 0  ) {

        sprintf( mime_type, "application/javascript" );
        is_static_resource = 1;

    } else if ( fnmatch("/*.png", use_url.c_str() , FNM_LEADING_DIR ) == 0  ) {

        sprintf( mime_type, "image/png" );
        is_static_resource = 1;    

    } else if ( fnmatch("/*.woff*", use_url.c_str() , FNM_LEADING_DIR ) == 0  ) {

        sprintf( mime_type, "application/font-woff" );
        is_static_resource = 1;
    
    } else if ( fnmatch("/*.svg", use_url.c_str() , FNM_LEADING_DIR ) == 0  ) {

        sprintf( mime_type, "image/svg+xml" );
        is_static_resource = 1;

    } else if ( fnmatch("/*.ico", use_url.c_str() , FNM_LEADING_DIR ) == 0  ) {

        sprintf( mime_type, "image/x-icon" );
        is_static_resource = 1;    

    } else if ( fnmatch("/*.json", use_url.c_str() , FNM_LEADING_DIR ) == 0 ) {

        sprintf( mime_type, "application/json" );
        is_web_service = 1;

    }




    if ( is_static_resource == 1 )   {

        ret = response_with_static_resource( connection, response, mime_type, use_url ) ;
    
    } else if ( is_web_service == 1 ) {

        ret = response_with_web_service( connection,  response , mime_type,  use_url );

    } else {

        ret = response_with_errormsg( connection, response, 401, "Unauthorized Request Type!" );
    }

    return ret;
}





//-----------------------------
bool 
Binawatch_httpd::has_pending_request() {

    return (request_queue.size() > 0 && get_current_epoch() > request_queue[0].exec_time );
}



//------------
void
Binawatch_httpd::process_request_queue() {

    if ( request_queue.size() > 0 ) {
        
        if ( get_current_epoch() >= request_queue[0].exec_time ) {

            if ( request_queue[0].task ==  "allBookTickers" ) {
                
                Binawatch_apicaller::get_allBookTickers();
                queue_request_item( get_current_epoch() + 30, "allBookTickers", NULL, NULL );
                
            }
            request_queue.erase( request_queue.begin() );
        }
    }
}

//----------------
// Queue to be dispatched at other thread so don't block the httpd thread.
void 
Binawatch_httpd::queue_request_item( 
    time_t exec_time,
    const char *task, 
    struct MHD_Connection *connection, 
    MHD_Response *response
) {

    write_log("<Binawatch_httpd::queue_request_item> %d %s", exec_time , task );

    int i;
    bool inserted = 0;

    struct request r; 
    r.exec_time = exec_time;
    r.task = string( task );
    r.connection = connection;
    r.response = response; 

    for ( i = 0 ; i < request_queue.size() ; i++ ) {
        if ( r.exec_time < request_queue[i].exec_time ) {
            request_queue.insert( request_queue.begin() + i , r);
            inserted = 1;
            break;
        }
    }
    if ( inserted == 0 ) {
        request_queue.push_back( r );
    }

}



//-------------
int 
Binawatch_httpd::stop( ) {

    write_log("<Binawatch_httpd::stop> Stopping HTTPD Daemon");

    if ( Binawatch_httpd::daemon != NULL ) {
        MHD_stop_daemon ( Binawatch_httpd::daemon);
        Binawatch_httpd::daemon = NULL;
    }
    return 0; 
}






//-----------------------------------------------
void 
Binawatch_httpd::write_log( const char *fmt, ... ) 
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



//-----------------------------------------------
// Write log to channel without any timestamp nor new line
void 
Binawatch_httpd::write_log_clean( const char *fmt, ... ) 
{
    va_list arg;
    va_start (arg, fmt);
    vfprintf ( stdout, fmt, arg);
    va_end (arg);

    fflush(stdout);

}




//--------------------------------------------------------
// Start the httpd server
int
Binawatch_httpd::init( int port )
{
    write_log("<Binawatch_httpd::init>");

    Binawatch_httpd::daemon = MHD_start_daemon ( 
                                MHD_USE_SELECT_INTERNALLY, 
                                port, 
                                NULL, 
                                NULL,
                                &Binawatch_httpd::answer_to_connection, 
                                NULL, 
                                MHD_OPTION_END
                            );


    if ( Binawatch_httpd::daemon == NULL ) {
        write_log("<Binawatch_httpd::init> HTTPD Daemon failed to run!");
        return -1;
    }   

    write_log("<Binawatch_httpd::init> HTTPD started at port %d", port );

    Binawatch_apicaller::shared_data = Binawatch_webservices::shared_data;
    

    return 0;   
}


