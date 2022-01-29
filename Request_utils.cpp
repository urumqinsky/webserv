#include "libraryHeaders.hpp"
#include "ServerConfig.hpp"

serCont *findServer(Request &other) {
    std::vector<serCont>::iterator it_begin = other.conf->serverList.begin();
    std::vector<serCont>::iterator it_end = other.conf->serverList.end();
    while(it_begin != it_end) {
    	if ((*it_begin).ip == other.ip && (*it_begin).port == other.port) {
    		if ((*it_begin).server_name == (other.headers.find("Host"))->second) 
            return &(*it_begin);
        }
        ++it_begin;
    }
    serCont *tmp = NULL;
    return tmp;
}


locCont *findLocation(Request &other) {
    other.pathConfCheck = other.path;
    serCont *ptr;
    while (other.pathConfCheck.length() > 0) {
        if ((ptr = findServer(other)) != NULL) {
            std::vector<locCont>::iterator it_begin = ptr->locListS.begin();
            std::vector<locCont>::iterator it_end = ptr->locListS.end();
            while (it_begin != it_end) {
                if ((*it_begin).locArgs[0] == other.pathConfCheck && std::count ((*it_begin).methods.begin(), (*it_begin).methods.end(), other.method) > 0) {
                    return &(*it_begin);
                }
                ++it_begin;
            }
        }
        int slash = other.pathConfCheck.rfind("/");
        if (slash == (int)other.pathConfCheck.find("/") && other.pathConfCheck.length() > 1)
            slash = 1;
        other.pathConfCheck.erase(slash);
    }
    locCont *tmp = NULL;
    return tmp;    
}

// std::string provaideDate() {

// }