#include "Request.hpp"

Request::Request(htCont *conf, lIpPort *ip) {
	this->status = START_LINE;
	this->errorCode = 200;
	this->chunkStatus = NUM;
	this->ip = ip->ip;
	this->port = ip->port;
	this->conf = conf;
	this->serverName = "webserv";
	setAllErrorCodes();
}

Request::~Request() {}

std::string const &Request::getipPort() { return this->ip; }

void Request::setipPort(int ipPort) { this->ip = ipPort; }

Status Request::getStatus() { return this->status; }

void Request::setStatus(Status status) { this->status = status; }

std::string Request::getResponce() { return this->responce; }

void Request::makeRequestDefault() {
	this->status = START_LINE;
	this->errorCode = 200;
	this->chunkStatus = NUM;
	this->method.erase();
	this->path.erase();
	this->http.erase();
	this->headers.erase(this->headers.begin(), this->headers.end());
	this->body.erase();
	this->responce.erase();
	this->respBody.erase();

}

void Request::setAllErrorCodes() {
	int code[] = {100, 101, 200, 201, 202, 203, 204, 205, 206, 300, 301, 302, 303, 304, 305, 307, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 426, 500, 501, 502, 503, 504, 505};
	std::string reason[] = {"CONTINUE", "SWITCHING PROTOCOLS", "OK", "CREATED", "ACCEPTED", "NON-AUTHORITATIVE INFORMATION", "NO CONTENT", "RESET CONTENT", "PARTIAL CONTENT", "MULTIPLE CHOICES", "MOVED PERMANENTLY", "FOUND", "SEE OTHER", "NOT MODIFIED", "USE PROXY", "TEMPORARY REDIRECT", "BAD REQUEST", "UNAUTHORIZED", "PAYMENT REQUIRED", "FORBIDDEN", "NOT FOUND", "METHOD NOT ALLOWED", "NOT ACCEPTABLE", "PROXY AUTHENTICATION REQUIRED", "REQUEST TIMEOUT", "CONFLICT", "GONE", "LENGTH REQUIRED", "PRECONDITION FAILED", "PAYLOAD TOO LARGE", "URI TOO LONG", "UNSUPPORTED MEDIA TYPE", "RANGE NOT SATISFIABLE", "EXPECTATION FAILED", "UPGRADE REQUIRED", "INTERNAL SERVER ERROR", "NOT IMPLEMENTED", "BAD GATEWAY", "SERVICE UNAVAILABLE", "GATEWAY TIMEOUT", "HTTP VERSION NOT SUPPORTED"};
	size_t size;
	for (size = 0; code[size] != 505; size++);
	for (size_t i = 0; i <= size; i++) {
		this->allErrorCodes.insert(std::pair<int, std::string>(code[i], reason[i]));
	}
}

void parseStartLine(Request &other) {
	std::string tmp = other.buf.substr(0, other.buf.find("\r\n"));
	if (tmp.empty()) {
		other.status = ERROR;
		other.errorCode = 404;
	} else {
		char *tmp2 = new char[tmp.length()+1];
		std::strcpy (tmp2, tmp.c_str());
		other.method = std::strtok(tmp2, " ");
		other.path = std::strtok(NULL, " ");
		other.http = std::strtok(NULL, " ");
		delete[] tmp2;
		if (!(other.method == "GET" || other.method == "POST" || other.method == "DELETE" || other.method == "HEAD")) {
			other.status = ERROR;
			other.errorCode = 501;
		} else if (other.http.empty()) {
			other.status = ERROR;
			std::cout << "error2" << std::endl;
		} else if (other.http != "HTTP/1.1") {
			other.status = ERROR;
			other.errorCode = 505;
		} else {
			other.buf.erase(0, tmp.length() + 2);
			other.status = HEADERS;
		}

	}
}

void parseHeader(Request &other) {
	std::string first;
	std::string second;
	if (other.buf.find("\r\n\r\n") != std::string::npos || other.buf == "\r\n")
		other.status = BODY;
	while (other.buf.find("\r\n") != std::string::npos && other.buf.find("\r\n") != 0) {
		first = other.buf.substr(0, other.buf.find(":"));
		size_t size = first.length();
		for (size_t i = 0; i < size; ++i)
			first[i] = (unsigned char)std::toupper(first[i]);
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
		std::string tmp = other.headers.find("HOST")->second;
		other.headers.find("HOST")->second = tmp.substr(0, tmp.find(":"));
	}
}

void parseBody(Request &other) { // check body for terminal
	if (other.buf.find("\r\n\r\n") != std::string::npos) {
		other.body.assign(other.buf, 0, other.buf.length() - 4);
		other.buf.erase();
	}
	if (other.method == "POST" && other.body.empty()) {
		// other.status = ERROR;
		other.errorCode = 204;
	} else {
		other.status = COMPLETED;
	}
}


void Request::parseFd(std::string req) {

	this->buf += req;
	if (this->buf.find("\r\n") != std::string::npos) {
		switch (this->status) {
			case START_LINE:
				parseStartLine(*this);
				if (this->status == START_LINE || this->status == ERROR)
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

/////////////////////////////PRINT:
	std::cout << this->method << "\t" << this->path << "\t" << this->http << std::endl;
	std::map<std::string, std::string>::iterator it2 = this->headers.begin();
	while (it2 != this->headers.end()) {
		std::cout << it2->first << " - " << it2->second << std::endl;
		++it2;
	}
	if (this->body != "")
		std::cout << this->body << std::endl;
/////////////////////////////PRINT_END
	if (this->status == COMPLETED || this->status == ERROR) {
		if((this->locConf = findLocation(*this)) == NULL) {
			// if (this->status != ERROR) {
			// 	this->status = ERROR;
			// 	this->errorCode = 404;
			// }
			std::cout << "checkRequest error. Location not found" << "\n";
		} else if (this->status != ERROR) {
			if (!this->locConf->cgiPath.empty() && !this->locConf->cgiExtension.empty() && checkIfCgi()) {
				cgiHandler();
				return ;
			} else {
				createBody();
			}
		}
		if (this->status == ERROR)
			createErrorBody();
		createResponce();
	}
	// sleep (10);
	}
}

bool Request::checkIfCgi() {
	std::string tmpCgiPath = this->locConf->cgiPath + this->locConf->cgiExtension;
	if (!tmpCgiPath.compare(0, 2, "./")) {
		tmpCgiPath.erase(0, 1);
	}
	if (this->path == tmpCgiPath) {
		std::string file = this->locConf->genL.root + "/" + this->locConf->cgiPath + this->locConf->cgiExtension;
		std::ifstream fs;
		fs.open(file);
		if (fs.is_open()) {
			fs.close();
			return 1;
		}
	}
	return 0;
}

std::string searchIndexFile(Request &other) {
	std::vector<std::string>::iterator it_begin = other.locConf->genL.index.begin();
	std::vector<std::string>::iterator it_end = other.locConf->genL.index.end();
	std::string indexFile;
	while (it_begin != it_end) {
		indexFile = readFromFile(other.locConf->genL.root + "/" + (*it_begin));
		if (!indexFile.empty()) {
			return indexFile;
		} else {
			++it_begin;
		}
	}
	return indexFile;
}

void autoindexOn(Request &other) {
	DIR *dir;
	struct dirent *file;
	std::string indexResponce;
	std::ifstream fs;
	std::string dirName = other.locConf->genL.root + other.path;

	if ((dir = opendir(dirName.c_str())) != NULL) {
		//show files in path:
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
		std::string tmp = readFromFile(dirName);
		if (!tmp.empty()) {
			// show file:
			other.respBody += "<html><head><title></title></head><body><p>" + tmp + "</p></body></html>\r\n";
		} else {
			other.status = ERROR;
			other.errorCode = 404;
			std::cout << "autoindex ERROR" << "\n";
		}
	}

}


void Request::createBody() {
	std::string indexFile;
	if (!this->locConf->genL.index.empty()) {
		indexFile = searchIndexFile(*this);
	}
	if (!indexFile.empty()){
		this->respBody = indexFile;
	} else if (this->locConf->genL.autoindex == 1) {
			autoindexOn(*this);
	} else {
		this->status = ERROR;
		this->errorCode = 404;
	}

}

void Request::createErrorBody() {
	if (this->locConf && this->locConf->genL.error_page.find(this->errorCode) != this->locConf->genL.error_page.end()) {
		std::string errorFile = this->locConf->genL.root + this->locConf->genL.error_page.find(this->errorCode)->second;
		this->respBody = readFromFile(errorFile);
		return ;
	}
	// this->respBody = "<html><head><title></title></head><body><p>ERROR PAGE IS NOT FOUND</p></body></html>\r\n";
}


void Request::createResponce() {
	std::stringstream tmpLength;
	tmpLength << this->respBody.size();
	std::string contLength = tmpLength.str();
	this->responce = this->http  + " " +  createStatusLine(this->errorCode, this->allErrorCodes) + "\r\n";
	this->responce += "Date: " + provaideDate() + "\r\n";
	this->responce += "Server: " + this->serverName + "\r\n";
	this->responce += "Content-Length:" +  contLength + "\r\n";
	this->responce += "Connection: Keep-Alive\r\n\r\n";
	this->responce += this->respBody;
	this->status = COMPLETED;


// PRINT RESPONCE
	std::cout << "\r\n" << this->responce << std::endl;
}

void	Request::cgiHandler()
{
	int fd[2]; // fd[0] - read, fd[1] - write
	if (pipe(fd) == -1)
		printError("pipe() error");
	int pid = fork();
	if (pid < 0)
		printError("fork() error");
	if (pid == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		//find cgi file and put it with full path to execve
		execve("/Users/ltulune/Desktop/webserv/hello.cprog", 0, 0);
		//
	}
	close(fd[1]);
	char buf[200];
	memset(buf, 0, 200);
	read(fd[0], buf, 200);
	close(fd[0]);
	this->responce = std::string(buf);
	this->status = COMPLETED;
}
