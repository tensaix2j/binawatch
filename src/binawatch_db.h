


#include <string>
#include <vector>

#include <sqlite3.h> 

using namespace std;

class Binawatch_db {

	static sqlite3 *db;
	
	public:
		static vector < vector < string > > results_set;
	
		static int exec_sql_cb(void *data, int argc, char **argv, char **azColName );
		static int exec_sql( const char* sql_fmt, vector <string> &sql_args ) ;
		static void init();
		static void log_results_set();
		static string sanitize_arg( string &raw_arg) ;
};


