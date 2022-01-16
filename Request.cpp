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

void Request::parseFd(int fd) {
    this->request.insert(std::pair<std::string, std::string>("method", "GET"));
    std::map<std::string, std::string>::iterator it = this->request.begin();
    std::cout << it->first << std::endl;
    std::cout << it->second << std::endl;
}

