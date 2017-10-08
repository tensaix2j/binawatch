
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


