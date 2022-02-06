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

std::string ifAlias(locCont *locConf, std::string path) {
    if (locConf->alias.empty()) {
        return path;
    } else {
        return locConf->alias;
    }

}

// std::string clearFromSlash(std::string str) {

// }

locCont *findLocation(Request &other) {
    locCont *tmp = NULL;
    struct stat buf;
    other.pathConfCheck = other.path;
    serCont *ptr;
    std::string appndx = "";
    while (other.pathConfCheck.length() > 0) {
        if ((ptr = findServer(other)) != NULL) {
            std::vector<locCont>::iterator it_begin = ptr->locListS.begin();
            std::vector<locCont>::iterator it_end = ptr->locListS.end();
            while (it_begin != it_end) {
                if ((*it_begin).locArgs[0] == other.pathConfCheck) {
                    if (std::count((*it_begin).methods.begin(), (*it_begin).methods.end(), other.method) == 0) {
                        // std::cout << "METHOD " << other.method << "\n";
                        other.status = ERROR;
                        other.errorCode = 405;
                    }
                    // other.aliasPath = ifAlias(&(*it_begin), other.path) + appndx;
                    // other.fullPath = (*it_begin).genL.root + "/" + other.aliasPath;
                    // std::string rP = (*it_begin).genL.root + "/" + other.aliasPath;
                    // const char *rootPath = rP.c_str();
                    other.aliasPath = ifAlias(&(*it_begin), other.pathConfCheck) + appndx;
                    other.fullPath = (*it_begin).genL.root + "/" + other.aliasPath;
                    const char *rootPath = other.fullPath .c_str();
                    stat(rootPath, &buf);

                    std::cout << rootPath << "<==========\n";
                    std::cout << S_ISREG(buf.st_mode) << "\n";
                    std::cout << S_ISDIR(buf.st_mode) << "\n";
                    if (!S_ISREG(buf.st_mode) && !S_ISDIR(buf.st_mode)) {
                        std::cout << "<+++++++++++++" << "\n";
                        other.status = ERROR;
                        other.errorCode = 404;
                        return tmp; 
                    }
                    return &(*it_begin);
                }
                ++it_begin;
            }
        }
        int slash = other.pathConfCheck.rfind("/");
        if (slash == (int)other.pathConfCheck.find("/") && other.pathConfCheck.length() > 1)
            slash = 1;
        if (slash != 1)
            appndx = other.pathConfCheck.substr(slash) + appndx;
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
