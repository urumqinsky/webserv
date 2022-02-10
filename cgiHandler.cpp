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

// GET SCRIPT NAME
std::string Request::getScriptName() {
	return this->locConf->cgiPath + this->locConf->cgiExtension;
}

// GET CLIENT IP
std::string Request::getClientIp() {
	return this->clientIpPort.ip;
}

// GET CLIENT POR
std::string Request::getClientPort() {
	return integerToString(this->clientIpPort.port);
}

// MAIN FUNCTION FOR HANDLING CGI
void	Request::cgiHandler() {
	// char **env = new char *[ENVNUMS + headers.size()];
	// env[ENVNUMS + headers.size()] = NULL;
	// int i(0);
	
	// SERVER ENV
	// env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	// env[i++] = strdup(("SERVER_NAME=" + getHeader("HOST", this->headers)).c_str());
	// env[i++] = strdup(("SERVER_PORT=" + integerToString(port)).c_str());
	// env[i++] = strdup(("SERVER_PROTOCOL=" + http).c_str());
	// env[i++] = strdup(("SERVER_SOFTWARE=" + std::string("testname")).c_str());

	// REQUEST ENV 
	// env[i++] = strdup(("CONTENT_LENGTH=" + getContentLenght()).c_str());
	// env[i++] = strdup(("CONTENT_TYPE=" + getContentType()).c_str());
	// env[i++] = strdup(("PATH_INFO=" + getPathInfo()).c_str());
	// env[i++] = strdup(("QUERY_STRING=" + getQueryString()).c_str());
	// env[i++] = strdup(("REMOTE_ADDR=" + getClientIp()).c_str());
	// env[i++] = strdup(("REMOTE_HOST=" + getClientPort()).c_str());
	// env[i++] = strdup(("REQUEST_METHOD=" + this->method).c_str());
	// env[i++] = strdup(("REQUEST_LINE=" + this->requestLine).c_str());
	// env[i++] = strdup(("SCRIPT_NAME=" + getScriptName()).c_str());

	// SERVER ENV
	// std::map<std::string, std::string>::iterator it2 = headers.end();
	// for (std::map<std::string, std::string>::iterator it = headers.begin(); it != it2; ++it) {
	// 	env[i++] = strdup(("HTTP_" + it->first + "=" + it->second).c_str());
	// }
	// fd[0] - read, fd[1] - write
	int fd[2];
	char *argv[2] = {const_cast<char *>(this->fullPath.c_str()), NULL };
	if (pipe(fd) == -1) {
		printError("pipe() error");
	}
	if (this->body.empty()) {
		if (write(fd[1], "name=default&login=default", 27) == -1)
			printError("write() error");
	} else {
		if (write(fd[1], this->body.c_str(), this->body.size()) == -1)
			printError("write() error");
	}
	int pid = fork();
	if (pid < 0) {
		printError("fork() error");
	} else if (pid == 0) {
		dup2(fd[0], STDIN_FILENO);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		int res = execve(argv[0], argv, 0);
		exit(res);
	}
	close(fd[1]);
	int res;
	waitpid(-1, &res, 0);
	char buf[10000];
	int count = 1;
	while (count > 0) {
		memset(buf, 0, 10000);
		count = read(fd[0], buf, 10000);
		if (count == -1)
			printError("read() error");
		this->respBody += buf;
	}
	close(fd[0]);
	this->status = COMPLETED;
}
