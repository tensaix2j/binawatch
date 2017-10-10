



#include <sqlite3.h> 

using namespace std;

class Binawatch_db {

	static sqlite3 *db;
		
	public:
		static int select_cb(void *data, int argc, char **argv, char **azColName );
		static int insert_cb(void *data, int argc, char **argv, char **azColName );
		static void select_record( const char* sql) ;
		static void insert_record( const char* sql) ;
		static void init();

};

