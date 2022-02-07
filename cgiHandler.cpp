#include "Request.hpp"
#include <sstream>

#define ENVNUMS 15

// CONVERT INTEGER TO STRING
template<typename T>
std::string integerToString(T number) {
	std::ostringstream ss;
	ss << number;
	return ss.str();
}

// PARSE AND GET QUERY_STRING
std::string Request::getQueryString() {
	std::string query;
	if (this->path.find("?") != std::string::npos) {

	} else {
		query = "";
	}
	return query;
}

// GET CONTENT_LENGTH
std::string Request::getContentLenght() {
	if (this->headers.find("CONTENT_LENGTH")->second.empty()) {
		return integerToString(this->body.length());
	} else {
		return this->headers.find("CONTENT_LENGTH")->second;
	}
}

// TMP FUNCTION FOR PRINTING HEADERS
void printHeaders(std::map <std::string, std::string> &headers) {
	std::map <std::string, std::string>::iterator it = headers.begin();
	std::map <std::string, std::string>::iterator it2 = headers.end();
	for (; it != it2; ++it) {
		std::cout << it->first << " == " << it->second << std::endl;
	}
	it = headers.begin();
	std::cout << "================================\n";
	for (; it != it2; it++) {
		std::cout << it->first << " == " << it->second << std::endl;
	}
}

// MAIN FUNCTION FOR HANDLING CGI
void	Request::cgiHandler()
{
	const char *env[ENVNUMS + headers.size() + 1];
	int i(0);

	std::cout << this->locConf->cgiPath + this->locConf->cgiExtension << std::endl;
	printHeaders(headers);
	
	// server env
	env[i++] = ("GATEWAY_INTERFACE=CGI/1.1");
	env[i++] = ("SERVER_NAME=" + headers.find("HOST")->second).c_str();
	env[i++] = ("SERVER_PORT=" + integerToString(port)).c_str();
	env[i++] = ("SERVER_PROTOCOL=" + http).c_str();
	env[i++] = ("SERVER_SOFTWARE=" + serverName).c_str();
	// request 
	env[i++] = ("CONTENT_LENGTH=" + getContentLenght()).c_str();
	env[i++] = ("CONTENT_TYPE=" + headers.find("CONTENT_TYPE")->second).c_str();
	env[i++] = ("PATH_INFO=");
	env[i++] = ("QUERY_STRING=" + getQueryString(this->path)).c_str();
	// env[i++] = ("REMOTE_ADDR=");
	// env[i++] = ("REMOTE_HOST |");
	env[i++] = ("REQUEST_METHOD=" + this->method).c_str();
	env[i++] = ("REQUEST_LINE=" + this->requestLine).c_str();
	// env[i++] = ("SCRIPT_NAME=" + getFileName()).c_str();
	env[ENVNUMS] = NULL;

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
		execve("/Users/rlando/Desktop/cgi.bla", 0, 0);
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
