
#include "binawatch_utils.h"

//--------------------------------
void split_string( string &s, char delim, vector <string> &result) {

    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    
}



//--------------------------------
bool replace_string( string& str, const char *from, const char *to) {

    size_t start_pos = str.find(from);
    if( start_pos == std::string::npos ) {
        return false;
    }
    str.replace(start_pos, strlen(from), to);
    return true;
}



//--------------------------------------
string b2a_hex( char *byte_arr, int n ) {

    const static std::string HexCodes = "0123456789abcdef";
    string HexString;
    for ( int i = 0; i < n ; ++i ) {
        unsigned char BinValue = byte_arr[i];
        HexString += HexCodes[( BinValue >> 4 ) & 0x0F];
        HexString += HexCodes[BinValue & 0x0F];
    }
    return HexString;
}



//---------------------------------
time_t get_current_epoch( ) {

    struct timeval tv;
    gettimeofday(&tv, NULL); 

    return tv.tv_sec ;
}

//---------------------------------
unsigned long get_current_ms_epoch( ) {

    struct timeval tv;
    gettimeofday(&tv, NULL); 

    return tv.tv_sec * 1000 + tv.tv_usec / 1000 ;

}

//---------------------------
string hmac_sha256( const char *key, const char *data) {

    unsigned char* digest;
    digest = HMAC(EVP_sha256(), key, strlen(key), (unsigned char*)data, strlen(data), NULL, NULL);    
    return b2a_hex( (char *)digest, 32 );
}   

