#include "Request.hpp"

//ipPort

// Request::Request() {
// 	//показать, какой сервер??
// 	this->status = START_LINE;
// 	this->chunkStatus = NUM;
// }

Request::Request(htCont *conf, lIpPort *ip) {
	this->status = START_LINE;
	this->chunkStatus = NUM;
	this->conf = conf;
	this->ip = ip->ip;
	this->port = ip->port;
}

Request::~Request() {}

std::string const &Request::getipPort() {
	return this->ip;
}

void Request::setipPort(int ipPort) {
	this->ip = ipPort;
}

void Request::setStatus(Status st)
{
	this->status = st;
}

Status Request::getStatus() {
	return this->status;
}

std::string Request::getResponce() {
	return this->responce;
}


void parseStartLine(Request &other) {
	std::string tmp = other.buf.substr(0, other.buf.find("\r\n"));
	char *tmp2 = new char[tmp.length()+1];
	std::strcpy (tmp2, tmp.c_str());
	other.method = std::strtok(tmp2, " ");
	other.path = std::strtok(NULL, " ");
	other.http = std::strtok(NULL, " ");
	delete[] tmp2;
	if (!(other.method == "GET" || other.method == "POST" || other.method == "DELETE")) {
		other.status = ERROR;
		std::cout << "501 - Not Implemented" << std::endl;
	} else if (other.http.empty()) {
		other.status = ERROR;
		std::cout << "error2" << std::endl;
	} else if (other.http != "HTTP/1.1") {
		other.status = ERROR;
		std::cout << "505 - HTTP Version Not Supported" << std::endl;
	} else {
		other.buf.erase(0, tmp.length() + 2);
		other.status = HEADERS;
	}
}

void parseHeader(Request &other) {
	std::string first;
	std::string second;
	if (other.buf.find("\r\n\r\n") != std::string::npos)
		other.status = BODY;
	while (other.buf.find("\r\n") != std::string::npos && other.buf.find("\r\n") != 0) {
		first = other.buf.substr(0, other.buf.find(":")); // uppercase
		second = other.buf.substr(other.buf.find(":") + 1, other.buf.find("\r\n") - first.length() - 1);
		other.headers.insert(std::pair<std::string, std::string>(first, second));
		other.buf.erase(0, first.length() + second.length() + 2 + 1);
	}
	if (other.status == BODY) {
		other.buf.erase(0, 2);
		std::map<std::string, std::string>::iterator it = other.headers.begin();
		while (it != other.headers.end()) {
			size_t i = 0;
			size_t j = it->second.length() - 1;
			for ( ; it->second.at(i) == ' '; i++);
			for ( ; it->second.at(j) == ' '; j--);
			it->second.assign(it->second, i, j - i + 1);
			++it;
		}
		std::string tmp = other.headers.find("Host")->second;
		other.headers.find("Host")->second = tmp.substr(0, tmp.find(":"));
	}
}

void parseBody(Request &other) {
	if (other.buf.find("\r\n\r\n") != std::string::npos) {
		other.body.assign(other.buf, 0, other.buf.length() - 4);
		other.buf.erase();
	}
	other.status = COMPLETED;
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
			case BODY:
				parseBody(*this);
			default:
				break;

		}
	}
//PRINT:
	std::cout << this->method << "\t" << this->path << "\t" << this->http << std::endl;
	std::map<std::string, std::string>::iterator it2 = this->headers.begin();
	while (it2 != this->headers.end()) {
		std::cout << it2->first << " - " << it2->second << std::endl;
		++it2;
	}
	if (this->body != "")
		std::cout << this->body << std::endl;
	// if (CGI)
	// 	go to Said(*this);
	// else
	createResponce();
	// sleep (10);
}

void checkRequest(Request &other) {
	(void)other;
	std::vector<serCont>::iterator it_begin = other.conf->serverList.begin();
	std::vector<serCont>::iterator it_end = other.conf->serverList.end();
	other.status = ERROR;
	while(it_begin != it_end && other.status != COMPLETED) {
		if ((*it_begin).ip == other.ip && (*it_begin).port == other.port && other.status != COMPLETED) {
			if ((*it_begin).server_name == (other.headers.find("Host"))->second) {
				for (size_t i = 0; i != (*it_begin).locListS.size() && other.status != COMPLETED; i++) {
					size_t j = 0;
					size_t vecEnd = (*it_begin).locListS[i].locArgs.size();
					for (; j <= vecEnd && (*it_begin).locListS[i].locArgs[j] != other.path; j++);
					if (j != vecEnd) {
						other.status = COMPLETED;
						std::cout << "HERE" << "\n";
						// break;
					}

				}
			}
			// else
				// continue;
		}
		else
			++it_begin;

	}
}

void Request::createResponce() {
	// checkRequest(*this);
	std::ifstream fs("/Users/heula/webserv/level1.html");
	std::string line;
	// this->responce = "HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-Type: text/html\r\n\r\n";
	this->responce = this->http + " 200 OK\r\nServer: webserv\r\nContent-Length:109\r\n";
	std::map<std::string, std::string>::iterator it = this->headers.begin();
	while (it != this->headers.end()) {
		this->responce += (it->first + ": " + it->second + "\r\n");
		++it;
	}
	this->responce += "\r\n";
	while (getline(fs, line))
		this->responce += line + "\r\n";

// PRINT RESPONCE
	// std::cout << "\r\n" << resp << std::endl;
}
