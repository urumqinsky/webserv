#include "Request.hpp"

unsigned long long Request::requestNumber = 0;

Request::Request(htCont *conf, lIpPort *ip) {
	this->requestNumber += 1;
	this->status = START_LINE;
	this->errorCode = 200;
	this->chunkStatus = NUM;
	this->bodySource = STR;
	this->ip = ip->ip;
	this->port = ip->port;
	this->conf = conf;
	this->serverName = "webserv";
	this->tmpBodySize = 0;
	this->chunkedBodySize = 0;
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
	this->bodySource = STR;
	this->method.erase();
	this->path.erase();
	this->http.erase();
	this->headers.erase(this->headers.begin(), this->headers.end());
	this->body.erase();
	this->responce.erase();
	this->respBody.erase();
	this->aliasPath.erase();
	this->fullPath.erase();
	this->pathConfCheck.erase();
	this->tmpBodySize = 0;
	this->chunkedBodySize = 0;
	this->serverName.erase();
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
	other.requestLine = other.buf.substr(0, other.buf.find("\r\n"));
	
	if (other.requestLine.empty()) {
		other.buf.erase(0, 2);
		other.status = MOVE_ON;
		return ;
	} else {
		char *tmp2 = new char[other.requestLine.length() + 1];
		std::strcpy (tmp2, other.requestLine.c_str());
		char *tmp3;
		if ((tmp3 = std::strtok(tmp2, " ")) != NULL) {
			other.method = tmp3;
			if ((tmp3 = std::strtok(NULL, " ")) != NULL) {
				other.path = tmp3;
				if ((tmp3 = std::strtok(NULL, " ")) != NULL) {
					other.http = tmp3;
				}
			}
		} else {
			other.status = ERROR;
			other.errorCode = 404;
		}
	if (other.method.empty() || other.path.empty() || other.http.empty()) {
		other.status = ERROR;
		other.errorCode = 404;
	}
		delete[] tmp2;
		if (!(other.method == "GET" || other.method == "POST" || other.method == "DELETE" || other.method == "HEAD" || other.method == "PUT")) {
			other.status = ERROR;
			other.errorCode = 501;
			other.buf.erase();
			return ;
		} else if (other.http.empty()) {
			other.errorCode = 400;
			other.status = ERROR;
			other.buf.erase();
		} else if (other.http != "HTTP/1.1") {
			other.status = ERROR;
			other.errorCode = 505;
			other.buf.erase();
		} else {
			other.buf.erase(0, other.requestLine.length() + 2);
			other.status = HEADERS;
		}
	}
}

void parseHeader(Request &other) {
	std::string first;
	std::string second;
	if (other.buf.find("\r\n\r\n") != std::string::npos || other.buf == "\r\n")
		other.status = HEAD_END;
	while (other.buf.find("\r\n") != std::string::npos && other.buf.find("\r\n") != 0) {
		first = other.buf.substr(0, other.buf.find(":"));
		size_t size = first.length();
		for (size_t i = 0; i < size; ++i) {
			if (first[i] == '-') {
				first[i] = '_';
			}
			first[i] = (unsigned char)std::toupper(first[i]);
		}
		second = other.buf.substr(other.buf.find(":") + 1, other.buf.find("\r\n") - first.length() - 1);
		other.headers.insert(std::pair<std::string, std::string>(first, second));
		other.buf.erase(0, first.length() + second.length() + 2 + 1);
	}
	if (other.status == HEAD_END) {
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
		if (getHeader("TRANSFER_ENCODING", other.headers) == "chunked") {
			other.status = CHUNKED_BODY;
			return ;
		}
		std::string contLength = getHeader("CONTENT_LENGTH", other.headers);
		size_t bodySize = atoi(contLength.c_str());
		if (!contLength.empty() && bodySize > 0) {
			other.tmpBodySize = bodySize;
			other.buf += "\r\n";
			other.status = BODY;
		} else
			other.status = COMPLETED;

	}
}

void writeToFile(Request &other, std::string str) {
	std::stringstream tmp;
	tmp << other.requestNumber;
	std::string fileName = "xyz" + tmp.str();

	std::fstream fs;
	fs.open(fileName, std::fstream::app);
	if (other.bodySource == STR) {
		other.chunkedBodySize = other.body.size();
		fs.write(other.body.c_str(), other.chunkedBodySize);
		other.body = fileName;
		other.bodySource = FD;
	}
	other.chunkedBodySize += str.size();
	fs.write(str.c_str(), str.size());
	fs.close();
}

void parseChunkedBody(Request &other) {
	size_t lineEnd;
	std::string str;
	while (other.status == CHUNKED_BODY && (lineEnd = other.buf.find("\r\n")) != std::string::npos) {
		if (other.chunkStatus == END && other.buf == "\r\n") {
			other.status = COMPLETED;
			if (other.chunkedBodySize == 0)
				other.chunkedBodySize = other.body.size();
			other.buf.erase(0, 2);
		}
		str = other.buf.substr(0, lineEnd);
		other.buf.erase(0, lineEnd + 2);
		if (other.chunkStatus == NUM) {
			if (str.empty()) {
				other.buf.erase(0, 2);
				return;
			}
			other.tmpBodySize = atol(str.c_str());
			if (other.tmpBodySize == 0)
				other.chunkStatus = END;
			else
				other.chunkStatus = TEXT;
		}
		else if (other.chunkStatus == TEXT) {
			if (str.size() > 10000 || other.bodySource == FD) {
				writeToFile(other, str);
			} else {
				other.body += str.assign(str, 0, other.tmpBodySize);
			}
			other.chunkStatus = NUM;
		}
	}
}

void parseBody(Request &other) {
	other.body += other.buf.substr(0, other.tmpBodySize - other.body.size());
	if (other.body.size() >= other.tmpBodySize) {
		other.status = COMPLETED;
	}
	other.buf.erase();
	if ((other.method == "POST") && other.body.empty()) {
		other.status = ERROR;
		other.errorCode = 400;
		other.buf.erase();
	}
}

bool checkRequest(Request &other) {
	size_t maxBodySize = other.locConf->genL.bodySizeMax;
	if (maxBodySize != 0 && (other.tmpBodySize > maxBodySize || other.chunkedBodySize > maxBodySize)) {
		other.status = ERROR;
		other.errorCode = 413;
		other.buf.erase();
		return 0;	
	}
	return 1;
}


void Request::parseFd(std::string req) {
	this->buf += req;
	while (this->buf.find("\r\n") != std::string::npos) {
		switch (this->status) {
			case START_LINE:
				parseStartLine(*this);
					break;
			case HEADERS:
				parseHeader(*this);
					break;
			case BODY:
				parseBody(*this);
					break;
			case CHUNKED_BODY:
				parseChunkedBody(*this);
					break;
			default:
				return;

		}
	}

	if (this->status == COMPLETED || this->status == ERROR) {
		if((this->locConf = findLocation(*this)) == NULL) {
			this->locConf = &(*(*this->conf->serverList.begin()).locListS.begin());
		} else if (this->status != ERROR) {
			if (!this->locConf->cgiPath.empty() && !this->locConf->cgiExtension.empty() && checkIfCgi()) {
				cgiHandler();
				createResponce();
				return ;
			} else if (checkRequest(*this)) {
				if (this->locConf->authorization == "on")
				{
					std::map<std::string, std::string>::iterator it = this->headers.find("Cookie");
					if (it == headers.end())
					{
						if (this->method != "POST" && (this->headers.find("COOKIE") == this->headers.end()))
						{
							this->respBody += "<!DOCTYPE html>\n<html>\n<head>\n";
							this->respBody += "<title>TOP SECRET</title>\n</head>\n<body>\n";
							this->respBody += "<h1>Please enter your login and password ";
							this->respBody += "<br>for accessing top secret files</h1>\n";
							this->respBody += "<form method=\"POST\" action=";
							this->respBody += this->path + '\n';
							this->respBody += "<label>username: <input type=\"text\" name=\"username\" autocomplete=\"username\" required></label><br>\n";
							this->respBody += "<label>password: <input type=\"password\" name=\"password\" autocomplete=\"current-password\" required></label><br>\n";
							this->respBody += "<button type=\"submit\">Login</button>\n</form>\n</body>\n</html>";
						}
						else if (this->method != "POST")
						{
							std::map<std::string, std::string>::iterator it = this->headers.find("COOKIE");
							std::string sessionID = it->second;
							sessionID = sessionID.substr(sessionID.find("=") + 1);
							it = this->locConf->users.begin();
							for ( ; it != this->locConf->users.end(); ++it)
							{
								if (it->second == sessionID)
									break ;
							}
							if (it != this->locConf->users.end())
								createBody();

						}
						else if (this->method == "POST")
						{
							char *tmp = new char[this->body.length() + 1];
							std::strcpy(tmp, this->body.c_str());
							std::string login = strtok(tmp, "&");
							size_t n = login.find("=");
							login = login.substr(n + 1);
							std::string password = strtok(NULL, "&");
							n = password.find("=");
							password = password.substr(n + 1);
							delete [] tmp;
							std::map<std::string, std::string>::iterator it = this->locConf->users.find(login + password);
							if (it != this->locConf->users.end())
							{
								this->cookie = "SESSION_ID=" + it->second;
								this->method = "GET";
								createBody();
							}
							else {
								this->errorCode = 401;
								this->status = ERROR;
							}
						}
					}
				}
				else
					createBody();
			}
		}
		if (this->status == ERROR)
			createErrorBody();
		createResponce();
	} else if (this->status == MOVE_ON) {
		this->status = COMPLETED;
	}
}

bool Request::checkIfCgi() {
	std::string tmpCgiPath = this->locConf->cgiPath + this->locConf->cgiExtension;
	if (!tmpCgiPath.compare(0, 2, "./")) {
		tmpCgiPath.erase(0, 1);
	}

	std::string toCheck = this->aliasPath.substr(this->aliasPath.rfind("/"));
	if (clearFromSlash(toCheck) == clearFromSlash(tmpCgiPath)) {
		return 1;
	}
	return 0;
}

std::string searchIndexFile(Request &other) {
	std::vector<std::string>::iterator it_begin = other.locConf->genL.index.begin();
	std::vector<std::string>::iterator it_end = other.locConf->genL.index.end();
	std::string indexFile;
	while (it_begin != it_end) {
		indexFile = readFromFile(other.fullPath + "/" + (*it_begin));
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
	std::string dirName = other.fullPath;

	if ((dir = opendir(dirName.c_str())) != NULL) {
		while ((file = readdir(dir)) != NULL) {
			std::string tmp (file->d_name);
			std::string slash = other.path[other.path.length() - 1] == '/' ? "" : "/";
			std::string tmp_path = other.path +  slash + tmp;
			tmp = "<p><a href = \"" + tmp_path + "\">" + tmp + "</a></p>" ;
			indexResponce += tmp;
		}
		other.respBody += "<html><head><title></title></head><body>" + indexResponce + "</body></html>\r\n";
		closedir(dir);
	} else {
			other.status = ERROR;
			other.errorCode = 404;
	}
}

void Request::deleteMethod () {
	std::fstream file;
	file.open(this->fullPath);
	if (file.is_open()) {
		file.close();
		std::remove(this->fullPath.c_str());
	} else {
		this->status = ERROR;
		this->errorCode = 404;
	}
}

void Request::getPostMethod () {
	std::string indexFile;
	std::string tmp = readFromFile(this->fullPath);
	if (!tmp.empty()) {
		this->respBody = tmp;
		return ;
	} else if (!this->locConf->genL.index.empty()) {
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

void Request::createBody() {

	if (this->method == "GET" || (this->method == "POST" && this->cookie.empty())) {
		getPostMethod();
		return ;
	}
	else if (this->method == "DELETE") {
		deleteMethod();
		return ;
	} else {
		this->status = ERROR;
		this->errorCode = 501;
	}
}

void Request::createErrorBody() {
	std::string errorFile;
	if (this->locConf && this->locConf->genL.error_page.find(this->errorCode) != this->locConf->genL.error_page.end()) {
		errorFile = this->locConf->genL.error_page.find(this->errorCode)->second;
	} else if (this->locConf && this->locConf->genL.error_page.find(404) != this->locConf->genL.error_page.end()) {
		errorFile = this->locConf->genL.error_page.find(404)->second;
	}
	this->respBody = readFromFile(errorFile);
}


void Request::createResponce() {
	std::string contLength = integerToString(this->respBody.size());
	
	this->responce = this->http  + " " +  createStatusLine(this->errorCode, this->allErrorCodes) + "\r\n";
	this->responce += "Date: " + provaideDate() + "\r\n";
	this->responce += "Server: " + this->serverName + "\r\n";
	this->responce += "Content-Length:" +  contLength + "\r\n";
	if (!this->cookie.empty())
	{
		this->responce += "Set-Cookie:" + this->cookie + "\r\n";
		this->cookie.erase();
	}
	this->responce += "Connection: Keep-Alive\r\n\r\n";
	this->responce += this->respBody;
	this->status = COMPLETED;
}

void Request::setClientIpPort(const lIpPort &other)
{
	clientIpPort = other;
}
