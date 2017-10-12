
#include "binawatch_db.h"
#include "binawatch_httpd.h"
#include "binawatch_utils.h"




sqlite3 *Binawatch_db::db = NULL;
vector < vector < string > > Binawatch_db::results_set;


//----------------------------------------
int 
Binawatch_db::exec_sql_cb ( 

		void *data, 
		int argc, 
		char **argv, 
		char **azColName 
	)
{

	int i;
	vector <string> row;

	for (i = 0; i < argc; i++ ){
    	//Binawatch_httpd::write_log("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
		string val = argv[i]? string(argv[i]) : "";
		row.push_back( val );
   	}
   	results_set.push_back( row );

	return 0;
}


//---------------------
// Some basic sanitization to make sql safer
string 
Binawatch_db::sanitize_arg( string &raw_arg) {

	Binawatch_httpd::write_log( "<Binawatch_db::sanitize_arg> %s", raw_arg.c_str() );
	
	// Do not allow arg too long
	if ( raw_arg.length() > 1024 ) {
		raw_arg = raw_arg.substr(0,1024);
	}

	// Escape single and double quotes 
	replace_string( raw_arg , "'", "&apos;" );
	replace_string( raw_arg , "\"", "&quot;" );
	replace_string( raw_arg , "<", "&lt;" );
	replace_string( raw_arg , ">", "&gt;" );
	replace_string( raw_arg , "&", "&amp;" );
	replace_string( raw_arg , "\\", "" );
		
	
	return raw_arg;

}

//----------------------------------
int
Binawatch_db::exec_sql( const char* sql_fmt , vector <string> &sql_args )
{
	int i;
	char *zErrMsg = 0;
	int offset = 0;

	string final_sql = string(sql_fmt);
	for ( i = 0 ; i < sql_args.size() ; i++ ) {

		string replacement("'");
		replacement.append( sanitize_arg( sql_args[i] ) );
		replacement.append("'");

		offset = replace_string_once( final_sql , "?", replacement.c_str() , offset );
		if ( offset == 0 ) {
			break;
		}
	}

    Binawatch_httpd::write_log( "<Binawatch_db::exec_sql> %s\n", final_sql.c_str() );
	
    results_set.clear();
    int rc = sqlite3_exec(db, final_sql.c_str() , Binawatch_db::exec_sql_cb , 0, &zErrMsg);
   	
	if( rc != SQLITE_OK ){
		Binawatch_httpd::write_log("<Binawatch_db::exec_sql> SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	} else {
		Binawatch_httpd::write_log( "<Binawatch_db::exec_sql> Records queried successfully\n");
	}
	return 0;
}


//---------
void 
Binawatch_db::log_results_set() 
{
	Binawatch_httpd::write_log("<Binawatch_db::log_results_set>\n\n");
		
	int i, j;

	for ( i = 0 ; i < results_set.size() ;i++ ) {
		for ( j = 0 ; j < results_set[i].size() ; j++ ) {
			Binawatch_httpd::write_log_clean("%s |" , results_set[i][j].c_str() );
		}
		Binawatch_httpd::write_log_clean("\n");
	}

}


//----------------------------------
void
Binawatch_db::init() 
{

	char *zErrMsg = 0;
	int rc = sqlite3_open("../db/binawatch.db3", &db);

	if( rc ) {
		Binawatch_httpd::write_log("Can't open database: %s\n", sqlite3_errmsg(db));
		
	} else {
      	Binawatch_httpd::write_log("Opened database successfully\n");
	}
}

