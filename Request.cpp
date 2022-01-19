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
	std::string tmp = req.substr(0, req.find('\n'));
	char *tmp2 = new char[tmp.length()+1];
	std::strcpy (tmp2, tmp.c_str());
	this->request.insert(std::pair<std::string, std::string>("method", std::strtok(tmp2, " "))); // method
	this->request.insert(std::pair<std::string, std::string>("path", std::strtok(NULL, " "))); // path
	this->request.insert(std::pair<std::string, std::string>("http", std::strtok(NULL, " "))); // http
	delete[] tmp2;

	// size_t space = req.find(' ');
	// this->request.insert(std::pair<std::string, std::string>("method", req.substr(0, space))); // method
	// // space += req.find(' ', space);
	// this->request.insert(std::pair<std::string, std::string>("path", req.substr(space, space + req.find(' ', space)))); // path
	// space += req.find(' ', space);
	// this->request.insert(std::pair<std::string, std::string>("http", req.substr(0, space))); // http

	size_t found = req.find("\n") + 1;
	// for (size_t found = req.find("\n"); found != std::string::npos; found += req.find("\n")) {
		// while ()
		this->request.insert(std::pair<std::string, std::string>(req.substr(found, req.find(": ")), req.substr(req.find(": ") + 1, req.find("\n")))); // 
		// this->request.insert(std::pair<std::string, std::string>(req.substr(0, req.find(": ")), req.substr(req.find(": ") + 1, req.find("\n")))); // 
// 
	// }



	std::map<std::string, std::string>::iterator it2 = this->request.begin();
	while (it2 != this->request.end()){
		std::cout << it2->first << " - " << it2->second << std::endl;
		++it2;
	}

	// std::cout << "\n" << req << std::endl << std::endl;

	// sleep (10);
}
