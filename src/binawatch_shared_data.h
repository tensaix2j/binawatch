
#ifndef BINAWATCH_SHARED_DATA 
#define BINAWATCH_SHARED_DATA

#include <map>
#include <string>

using namespace std;

struct Binawatch_shared_data {

	map <string,double> 	bidPrice;
	map <string,double> 	askPrice;
	map <string,double> 	bidQty;
	map <string,double> 	askQty;
	time_t 					last_query_time;

};

#endif