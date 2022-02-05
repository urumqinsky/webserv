#include "libraryHeaders.hpp"
#include "ServerConfig.hpp"

serCont *findServer(Request &other) {
    std::vector<serCont>::iterator it_begin = other.conf->serverList.begin();
    std::vector<serCont>::iterator it_end = other.conf->serverList.end();
    while(it_begin != it_end) {
    	if ((*it_begin).ip == other.ip && (*it_begin).port == other.port) {
    		if ((*it_begin).server_name == (other.headers.find("HOST"))->second) 
            return &(*it_begin);
        }
        ++it_begin;
    }
    serCont *tmp = NULL;
    return tmp;
}


locCont *findLocation(Request &other) {
    locCont *tmp = NULL;
    // DIR *dir;
        // other.status = ERROR;
        // other.errorCode = 404;
        // return tmp;
    // }
    other.pathConfCheck = other.path;
    serCont *ptr;
    while (other.pathConfCheck.length() > 0) {
        if ((ptr = findServer(other)) != NULL) {
            std::vector<locCont>::iterator it_begin = ptr->locListS.begin();
            std::vector<locCont>::iterator it_end = ptr->locListS.end();
            while (it_begin != it_end) {
                if ((*it_begin).locArgs[0] == other.pathConfCheck) {
                    std::string rootPath = (*it_begin).genL.root + other.path;
                    std::cout << rootPath << "<==========\n";
                    // if (!(access(rootPath.c_str(), F_OK) == 0 || (dir = opendir(rootPath.c_str())) = NULL)) {
                    // if ((dir = opendir(rootPath.c_str())) == NULL && access(rootPath.c_str(), F_OK) == -1) {
                    //     other.status = ERROR;
			        // 	other.errorCode = 404;
                    //     return tmp;
                    // }
                    if (std::count((*it_begin).methods.begin(), (*it_begin).methods.end(), other.method) == 0) {
                        // std::cout << "METHOD" << other.method << "\n";
                        other.status = ERROR;
                        other.errorCode = 405;
                    }
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
    return tmp;    
}

std::string provaideDate() {
    time_t now = time(0);
    tm *gmt = gmtime(&now);
    std::string curTime = asctime(gmt);
    curTime = curTime.erase(curTime.length() - 1) + " GMT";
    return curTime;
}

std::string createStatusLine(int code, std::map<int, std::string> &m) {
    std::stringstream codeStr;
    codeStr << code;
    std::string result = codeStr.str() + " " + m.find(code)->second;
    return result;
}

std::string readFromFile(std::string file) {
		std::ifstream fs(file);
        // fs.open(file);
		std::string buf;
		std::string tmp;
        if (fs.good()) {
            while (getline(fs, buf))
                tmp += buf;
            fs.close();
            return tmp;
        }
        return tmp;

}
