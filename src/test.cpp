
#include <stdio.h>
#include <sys/time.h>

#include <iostream>
#include <vector>

using namespace std;

//---------------------------------
unsigned int get_current_epoch( ) {

    struct timeval tv;
    gettimeofday(&tv, NULL); 

    return tv.tv_sec ;
}

//------------------
void insert_vec( vector <int> &v, int val ) {

	int i; 
	bool inserted = 0;
	for ( i = 0 ; i < v.size() ; i++ ) {
		if ( val < v[i] ) {
			v.insert( v.begin() + i , val );
			inserted = 1;
			break;
		}
	}
	if ( inserted == 0 ) {
		v.push_back( val ) ;
	}

}


int main() {

	vector <int> v;
	
	insert_vec( v , 100 );
	insert_vec( v , 10 );
	insert_vec( v , 200 );
	insert_vec( v , 14 );
	insert_vec( v , 1 );
	insert_vec( v , 5 );
	insert_vec( v , 115 );
	insert_vec( v , 14 );
	
	
			

	for ( int i = 0 ; i < v.size() ; i++ ) {
		cout << v[i] << endl;

	}


	return 0;
}