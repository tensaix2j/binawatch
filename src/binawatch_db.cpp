
#include "binawatch_db.h"
#include "binawatch_httpd.h"
#include "binawatch_utils.h"




sqlite3 *Binawatch_db::db = NULL;

	
//------------------------
int
Binawatch_db::select_cb (
		void *data, 
		int argc, 
		char **argv, 
		char **azColName 
	)
{
	int i;
	for (i = 0; i < argc; i++ ){
    	Binawatch_httpd::write_log("%s = %s", azColName[i], argv[i] ? argv[i] : "NULL");
   	}
   	return 0;
}


//----------------------------------------
int 
Binawatch_db::insert_cb ( 

		void *data, 
		int argc, 
		char **argv, 
		char **azColName 
	)
{
	return 0;
}



//----------------------------------
void
Binawatch_db::select_record( const char* sql) 
{
	char *zErrMsg = 0;
	int rc = sqlite3_exec(db, sql, Binawatch_db::select_cb , 0, &zErrMsg);
   	
	if( rc != SQLITE_OK ){
		Binawatch_httpd::write_log("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		Binawatch_httpd::write_log( "Records queried successfully\n");
	}
}



//----------------------------------
void
Binawatch_db::insert_record( const char* sql) 
{
	char *zErrMsg = 0;
	int rc = sqlite3_exec(db, sql, Binawatch_db::insert_cb , 0, &zErrMsg);
   	
	if( rc != SQLITE_OK ){
		Binawatch_httpd::write_log("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		Binawatch_httpd::write_log( "Records created successfully\n");
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

