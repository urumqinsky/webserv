#include "Request.hpp"
#include <sstream>

#define ENVNUMS 14

// CONVERT INTEGER TO STRING
template<typename T>
std::string integerToString(T number) {
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

// PARSE AND GET QUERY_STRING
std::string Request::getQueryString() {
	size_t pos = this->path.find("?");
	if (pos != std::string::npos) {
		return this->path.substr(pos + 1, this->path.length());
	} else {
		return "";
	}
}

// GET PATH INFO
std::string Request::getPathInfo() {
	std::string res;
	size_t pos = this->path.find(getScriptName());
	if (pos != std::string::npos) {
		res = path.substr(pos, path.size());
		pos = res.find("/");
        if (pos != std::string::npos) {
            return res.substr(pos, res.size());;
        }
	}
	return "";
}

// GET CONTENT_LENGTH
std::string Request::getContentLenght() {
	std::map<std::string, std::string>::iterator it = this->headers.find("CONTENT_LENGTH");
	if (it != this->headers.end()) {
		return this->headers.find("CONTENT_LENGTH")->second;
	} else {
		return integerToString(this->body.length());
	}
}

// GET CONTENT TYPE
std::string Request::getContentType() {
	std::map<std::string, std::string>::iterator it = this->headers.find("CONTENT_TYPE");
	if (it != this->headers.end()) {
		return this->headers.find("CONTENT_TYPE")->second;
	} else {
		return "";
	}
}

// TMP FUNCTION FOR PRINTING HEADERS
void printHeaders(std::map <std::string, std::string> &headers) {
	std::map<std::string, std::string>::iterator it = headers.begin();
	std::map<std::string, std::string>::iterator it2 = headers.end();
	for (; it != it2; ++it) {
		std::cout << it->first << " == " << it->second << std::endl;
	}
	it = headers.begin();
	std::cout << "================================\n";
	for (; it != it2; it++) {
		std::cout << it->first << " == " << it->second << std::endl;
	}
}

// GET SCRIPT NAME
std::string Request::getScriptName() {
	return this->locConf->cgiPath + this->locConf->cgiExtension;
}

std::string Request::getClientIp() {
	return this->clientIpPort.ip;
}

std::string Request::getClientPort() {
	return integerToString(this->clientIpPort.port);
}

// MAIN FUNCTION FOR HANDLING CGI
void	Request::cgiHandler()
{
	std::cout << ENVNUMS + headers.size() + 1 << std::endl;
	// const char *env[ENVNUMS + headers.size()];
	char **env = new char *[ENVNUMS + headers.size()];
	int i(0);

	std::cout << this->locConf->cgiPath + this->locConf->cgiExtension << std::endl;
	// printHeaders(headers);
	
	// SERVER ENV
	env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[i++] = strdup(("SERVER_NAME=" + getHeader("HOST", this->headers)).c_str());
	env[i++] = strdup(("SERVER_PORT=" + integerToString(port)).c_str());
	env[i++] = strdup(("SERVER_PROTOCOL=" + http).c_str());
	env[i++] = strdup(("SERVER_SOFTWARE=" + serverName).c_str());

	// REQUEST ENV 
	env[i++] = strdup(("CONTENT_LENGTH=" + getContentLenght()).c_str());
	env[i++] = strdup(("CONTENT_TYPE=" + getContentType()).c_str());
	env[i++] = strdup(("PATH_INFO=" + getPathInfo()).c_str());
	env[i++] = strdup(("QUERY_STRING=" + getQueryString()).c_str());
	env[i++] = strdup(("REMOTE_ADDR=" + getClientIp()).c_str());
	env[i++] = strdup(("REMOTE_HOST=" + getClientPort()).c_str());
	env[i++] = strdup(("REQUEST_METHOD=" + this->method).c_str());
	env[i++] = strdup(("REQUEST_LINE=" + this->requestLine).c_str());
	env[i++] = strdup(("SCRIPT_NAME=" + getScriptName()).c_str());

	// SERVER ENV
	std::map<std::string, std::string>::iterator it2 = headers.end();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != it2; ++it) {
		env[i++] = strdup(("HTTP_" + it->first + "=" + it->second).c_str());
	}
	env[ENVNUMS + headers.size()] = NULL;

	for (unsigned int t = 0; t < (ENVNUMS + headers.size()); t++) {
		std::cout << env[t] << " " << std::endl;
	}

	// BODY HANDLER
	char *argv[2] = {const_cast<char *>(this->fullPath.c_str()), NULL };

	int fd[2]; // fd[0] - read, fd[1] - write
	if (pipe(fd) == -1) {
		printError("pipe() error");
	}
	int pid = fork();
	if (pid < 0) {
		printError("fork() error");
	}
	if (pid == 0) {
		std::cout << "hello from child" << std::endl;
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		std::cout << "hello from child" << std::endl;
		//find cgi file and put it with full path to execve
		execve(argv[0], argv, env);
		//
	}
	close(fd[1]);
	char buf[200];
	int count = 1;
	while (count > 0) {
		memset(buf, 0, 200);
		count = read(fd[0], buf, 200);
		this->responce += buf;
	}
	close(fd[0]);
	std::cout << this->responce.length() << std::endl;
	// this->responce = std::string(buf);
	this->status = COMPLETED;
}
