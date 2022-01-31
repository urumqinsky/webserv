#include "Request.hpp"

Request::Request(htCont *conf, lIpPort *ip) {
	this->status = START_LINE;
	this->chunkStatus = NUM;
	this->ip = ip->ip;
	this->port = ip->port;
	this->conf = conf;
	this->serverName = "webserv";
}

Request::~Request() {}

std::string const &Request::getipPort() { return this->ip; }

void Request::setipPort(int ipPort) { this->ip = ipPort; }

Status Request::getStatus() { return this->status; }

void Request::setStatus(Status status) { this->status = status; }

std::string Request::getResponce() { return this->responce; }

void Request::makeRequestDefault() {
	this->status = START_LINE;
	this->chunkStatus = NUM;
	this->method.erase();
	this->path.erase();
	this->http.erase();
	this->headers.erase(this->headers.begin(), this->headers.end());
	this->body.erase();
	this->responce.erase();
	this->respBody.erase();

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
	if (other.buf.find("\r\n\r\n") != std::string::npos || other.buf == "\r\n")
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

void parseBody(Request &other) { // check body for terminal
	if (other.buf.find("\r\n\r\n") != std::string::npos) {
		other.body.assign(other.buf, 0, other.buf.length() - 4);
		other.buf.erase();
	}
	other.status = COMPLETED;
}


void Request::parseFd(std::string req) {

	this->buf += req;
	if (this->buf.find("\r\n") != std::string::npos) {
		switch (this->status) {
			case START_LINE:
				parseStartLine(*this);
				if (this->status == START_LINE)
					break;
			case HEADERS:
				parseHeader(*this);
				if (this->status == HEADERS)
					break;
			case BODY:
				parseBody(*this);
				if (this->status == BODY)
					break;
			default:
				break;

		}
	}
/////////////////////////////PRINT:
	// std::cout << this->method << "\t" << this->path << "\t" << this->http << std::endl;
	// std::map<std::string, std::string>::iterator it2 = this->headers.begin();
	// while (it2 != this->headers.end()) {
	// 	std::cout << it2->first << " - " << it2->second << std::endl;
	// 	++it2;
	// }
	// if (this->body != "")
	// 	std::cout << this->body << std::endl;
/////////////////////////////PRINT_END
	if (this->status == COMPLETED) {
		checkRequest(*this);
		if (this->status == ERROR) {
			std::cout << "create responce ERROR" << "\n";
			return ;
		}
		if (!this->locConf->cgiPath.empty() && !this->locConf->cgiExtension.empty() && checkIfCgi()) {
		// 	go to Said(*this);
			std::cout << "CGI\n";
		} else {
			createResponce();
		}
	}
	// sleep (10);
}

bool Request::checkIfCgi() {
	std::string file = this->locConf->genL.root + "/" + this->locConf->cgiPath + "/" + this->locConf->cgiExtension;
	std::ifstream fs(file);
	if (fs.good()) {
		return 1;
	} else {
		return 0;
	}
}

void checkRequest(Request &other) {
	if((other.locConf = findLocation(other)) == NULL) {
		other.status = ERROR;
		std::cout << "checkRequest error. Location not found" << "\n";
	}

}

std::string searchIndexFile(Request &other) {
	std::vector<std::string>::iterator it_begin = other.locConf->genL.index.begin();
	std::vector<std::string>::iterator it_end = other.locConf->genL.index.end();

	while (it_begin != it_end) {
		try {
			std::string indexFile = readFromFile(other.locConf->genL.root + "/" + (*it_begin));
			return indexFile;

		} catch (int a) {
			std::cout << "no index file" << "\n";
		}

		++it_begin;
	}
	return NULL;
}


void autoindexOn(Request &other) {
	DIR *dir;
	struct dirent *file;
	std::string indexResponce;
	std::ifstream fs;
	std::string dirName = other.locConf->genL.root + other.path;

	if ((dir = opendir(dirName.c_str())) != NULL) {
		while ((file = readdir(dir)) != NULL) {
			std::string tmp (file->d_name);
			std::string slash = other.path[other.path.length() - 1] == '/' ? "" : "/";
			std::string tmp_path = other.path + slash + tmp;
			tmp = "<p><a href = \"" + tmp_path + "\">" + tmp + "</a></p>" ;
			indexResponce += tmp;
		}
		other.respBody += "<html><head><title></title></head><body>" + indexResponce + "</body></html>\r\n";
		closedir(dir);
	} else {
		try {
			std::string tmp = readFromFile(dirName);
			other.respBody += "<html><head><title></title></head><body><p>" + tmp + "</p></body></html>\r\n";
		} catch (std::ios_base::failure) {
			other.status = ERROR;
			std::cout << "autoindex ERROR" << "\n";
		}
	}
	
}


void Request::createResponce() {
	std::string indexFile;
	if (!this->locConf->genL.index.empty()) {
		indexFile = searchIndexFile(*this);
	}
	if (!indexFile.empty()){
		this->respBody= indexFile;
	} else if (this->locConf->genL.autoindex == 1) {
			autoindexOn(*this);
	} else {
		this->status = ERROR;
	}

	std::stringstream tmpLength;
	tmpLength << this->respBody.size();
	std::string contLength = tmpLength.str();
	this->responce = this->http + " 200 OK\r\n";
	this->responce += "Date: " + provaideDate() + " GMT\r\n";
	this->responce += "Server: " + this->serverName + "\r\n";
	this->responce += "Content-Length:" +  contLength + "\r\n";
	this->responce += "Connection: Keep-Alive\r\n\r\n";
	this->responce += this->respBody;


// PRINT RESPONCE
	// std::cout << "\r\n" << resp << std::endl;
}
