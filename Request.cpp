#include "Request.hpp"

Request::Request() {
	//показать, какой сервер??
	this->status = FIRST_LINE;
	this->chunkStatus = NUM;
}

Request::~Request() {}




std::string const &Request::getIPport() {
	return this->IPport;
}

void Request::setIPport(int IPport) {
	this->IPport = IPport;
}

void Request::parseFd(std::string req) {

	this->buf += req;

	std::string tmp = this->buf.substr(0, this->buf.find('\n'));
	char *tmp2 = new char[tmp.length()+1];
	std::strcpy (tmp2, tmp.c_str());
	this->headers.insert(std::pair<std::string, std::string>("method", std::strtok(tmp2, " "))); // method
	this->headers.insert(std::pair<std::string, std::string>("path", std::strtok(NULL, " "))); // path
	this->headers.insert(std::pair<std::string, std::string>("http", std::strtok(NULL, " "))); // http
	delete[] tmp2;

	// size_t space = this->buf.find(' ');
	// this->headers.insert(std::pair<std::string, std::string>("method", this->buf.substr(0, space))); // method
	// // space += this->buf.find(' ', space);
	// this->headers.insert(std::pair<std::string, std::string>("path", this->buf.substr(space, space + this->buf.find(' ', space)))); // path
	// space += this->buf.find(' ', space);
	// this->headers.insert(std::pair<std::string, std::string>("http", this->buf.substr(0, space))); // http

	// size_t found = this->buf.find("\r\n");
	// // for (size_t found = this->buf.find("\n"); found != std::string::npos; found += this->buf.find("\n")) {
	// 	while (found != std::string::npos){
	// 	this->headers.insert(std::pair<std::string, std::string>(this->buf.substr(found + 1, this->buf.find(": ")), this->buf.substr(this->buf.find(": ") + 1, this->buf.find("\r\n")))); // 
	// 	// this->headers.insert(std::pair<std::string, std::string>(this->buf.sчubstr(0, this->buf.find(": ")), this->buf.substr(this->buf.find(": ") + 1, this->buf.find("\n")))); // 
	// 	found += this->buf.find("\r\n", found);
	// }



	std::map<std::string, std::string>::iterator it2 = this->headers.begin();
	while (it2 != this->headers.end()){
		std::cout << it2->first << " - " << it2->second << std::endl;
		++it2;
	}

	// std::cout << "\n" << this->buf << std::endl << std::endl;

	// sleep (10);
}
