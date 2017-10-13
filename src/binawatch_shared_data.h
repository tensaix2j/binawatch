
#ifndef BINAWATCH_SHARED_DATA 
#define BINAWATCH_SHARED_DATA

#include <map>
#include <string>

using namespace std;

struct Binawatch_shared_data {

	map <string,string> 	bidPrice;
	map <string,string> 	askPrice;
	map <string,string> 	bidQty;
	map <string,string> 	askQty;
	time_t 					last_query_time;

};

#endif