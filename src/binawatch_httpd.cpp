

#include "binawatch_httpd.h"
#include "binawatch_utils.h"
#include "binawatch_shared_data.h"

struct Binawatch_shared_data* Binawatch_httpd::shared_data = NULL;
struct MHD_Daemon * Binawatch_httpd::daemon = NULL;
    

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
        const char *url,
        struct stat *sbuf
    ) 
{

    int fd;
    int ret;
    
    char full_path[1024];
    sprintf( full_path , "../web%s", url );
    
    if ( (-1 == (fd = open ( full_path , O_RDONLY))) || (0 != fstat (fd, sbuf)) ) {
        if (fd != -1) {
            (void) close (fd);
        }
        ret = response_with_errormsg( connection,  response, 404 , "File Not Found" );
    }


    write_log("<Binawatch_httpd::answer_to_connection> Ready to serve %s", full_path );


    response = MHD_create_response_from_fd_at_offset64 (sbuf->st_size, fd, 0);
    MHD_add_response_header (response, "Content-Type", mime_type);
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);

    return ret;
}






//---------------------------------
int
Binawatch_httpd::response_with_dynamic_response( 
        struct MHD_Connection *connection, 
        MHD_Response *response
    ) 
{

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
    string str_response = fastWriter.write(tickers_arr) ;


    int ret;
  
    response = MHD_create_response_from_buffer ( str_response.size() , (void *)( str_response.c_str() ) , MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);

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
    struct stat sbuf;
    int ret;
    bool is_static_resource = 0;
    char mime_type[128] = "text/plain";
        


    if ( fnmatch( "/*.css", url, FNM_PATHNAME ) == 0 ) {
    
        sprintf( mime_type, "text/css" );
        is_static_resource = 1;
    
    } else if ( fnmatch("/*.html", url, FNM_PATHNAME ) == 0 ) {
        sprintf( mime_type, "text/html" );
        is_static_resource = 1;
    
    } else if ( fnmatch("/*.js", url , FNM_PATHNAME ) == 0  ) {

        sprintf( mime_type, "text/plain" );
        is_static_resource = 1;

    } else if ( fnmatch("/*.png", url , FNM_PATHNAME ) == 0  ) {

        sprintf( mime_type, "image/png" );
        is_static_resource = 1;    
    
    } else if ( fnmatch("/*.svg", url , FNM_PATHNAME ) == 0  ) {

        sprintf( mime_type, "image/svg" );
        is_static_resource = 1;    
    }





    if ( is_static_resource == 1 )   {

        ret = response_with_static_resource( connection,  response, mime_type, url , &sbuf) ;
    
    } else if ( strcmp( url, "/" ) == 0 ) {

        sprintf( mime_type, "text/html" );
        ret = response_with_static_resource( connection,  response, mime_type, "/binawatch.html" , &sbuf) ;
            

    } else if ( strcmp( url , "/binawatch_tickers_json" ) == 0 ) {

        ret = response_with_dynamic_response( connection,  response );

    } else {

        ret = response_with_errormsg( connection, response, 401, "Unauthorized Request Type!" );

    }

    return ret;
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

    return 0;   
}


