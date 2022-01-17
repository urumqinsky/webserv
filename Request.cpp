#include "Request.hpp"

Request::Request() {}

Request::~Request() {}

Request::Request(const Request &other) {
    *this = other;
}

Request	&Request::operator=(const Request &other) {
    this->IPport = other.IPport;
    this->request = other.request;
    this->responce = other.responce;
    return *this;
}

std::string const &Request::getIPport() {
    return this->IPport;
}

void Request::setIPport(int IPport) {
    this->IPport = IPport;
}

void Request::parseFd(std::string req) {
    std::string::iterator it = req.begin();
    int ptr = req.find(' ');
    this->request.insert(std::pair<std::string, std::string>("method", req.substr(0, ptr))); // method
    // it += req.find(' ');
    // ptr += req.at(ptr).find(' ');
    // this->request.insert(std::pair<std::string, std::string>("path", req.substr(ptr, (*it).find(' ')))); // path

    std::map<std::string, std::string>::iterator it2 = this->request.begin();
    while (it2 != this->request.end()){
        std::cout << it2->first << " - " << it2->second << std::endl;
        ++it2;
    }

    std::cout << "\n" << req << std::endl << std::endl;

    
}

