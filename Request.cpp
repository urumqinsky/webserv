#include "Request.hpp"

Request::Request() {
	//показать, какой сервер??
	this->status = START_LINE;
	this->chunkStatus = NUM;
}

Request::~Request() {}

std::string const &Request::getIPport() {
	return this->IPport;
}

void Request::setIPport(int IPport) {
	this->IPport = IPport;
}

Status Request::getStatus() {
	return this->status;
}

void parseStartLine(Request &other){
	std::string tmp = other.buf.substr(0, other.buf.find("\r\n"));
	char *tmp2 = new char[tmp.length()+1];
	std::strcpy (tmp2, tmp.c_str());
	other.method = std::strtok(tmp2, " ");
	other.path = std::strtok(NULL, " ");
	other.http = std::strtok(NULL, " ");
	delete[] tmp2;
	if (!(other.method == "GET" || other.method == "POST" || other.method == "DELETE")){
		other.status = ERROR;
		std::cout << "501 - Not Implemented" << std::endl; 
	}
	else if (other.http.empty()) {
		other.status = ERROR;
		std::cout << "error2" << std::endl;
	}
	else if (other.http != "HTTP/1.1") {
		other.status = ERROR;
		std::cout << "505 - HTTP Version Not Supported" << std::endl;
	}
	else {
		other.buf.erase(0, tmp.length() + 2);
		other.status = HEADERS;
	}
}

void parseHeader(Request &other) {
	std::string first;
	std::string second;
	if (other.buf.find("\r\n\r\n") != std::string::npos) 
		other.status = COMPLETED;
	while (other.buf.find("\r\n") != std::string::npos && other.buf.length() > 2) {
		first = other.buf.substr(0, other.buf.find(": ")); // space
		second = other.buf.substr(other.buf.find(": ") + 2, other.buf.find("\r\n") - first.length() - 2); //space X 2
		other.headers.insert(std::pair<std::string, std::string>(first, second));
		other.buf.erase(0, first.length() + second.length() + 2 + 2); // space
	}
	if (other.status == BODY){
		other.buf.erase(0, 2);
		// + delete all spaces
		// std::map<std::string, std::string>::iterator it = other.headers.begin();
		// while (it != other.headers.end()) {

		// }
	}

}


void Request::parseFd(std::string req) {

	this->buf += req;
	if (this->buf.find("\r\n") != std::string::npos) {
		// std::cout << req << std::endl; ///// <---------
		switch (this->status) {
			case START_LINE:
				parseStartLine(*this);
			case HEADERS:
				parseHeader(*this);
			default:
				break;

		}
	}

//PRINT:
	std::cout << this->method << "\t" << this->path << "\t" << this->http << std::endl;
	std::map<std::string, std::string>::iterator it2 = this->headers.begin();
	while (it2 != this->headers.end()){
		std::cout << it2->first << " - " << it2->second << std::endl;
		++it2;
	}


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




	// std::cout << "\n" << this->buf << std::endl << std::endl;

	// sleep (10);
}
