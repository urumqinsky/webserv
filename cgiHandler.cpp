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

	std::cout << "HELLO" << std::endl;

	char **env = new char *[ENVNUMS + headers.size()];
	env[ENVNUMS + headers.size()] = NULL;
	int i(0);
	
	// SERVER ENV
	env[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	env[i++] = strdup(("SERVER_NAME=" + getHeader("HOST", this->headers)).c_str());
	env[i++] = strdup(("SERVER_PORT=" + integerToString(port)).c_str());
	env[i++] = strdup(("SERVER_PROTOCOL=" + http).c_str());
	env[i++] = strdup(("SERVER_SOFTWARE=" + std::string("testname")).c_str());

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

	int bodyFileFd;
	char *argv[2] = {const_cast<char *>(this->fullPath.c_str()), NULL };

	FILE	*outFile = tmpfile();
	FILE	*inFile = tmpfile();

	long	fdOut = fileno(outFile);
	long	fdIn = fileno(inFile);
	if (this->bodySource == STR) {
		write(fdIn, this->body.c_str(), this->body.size());
		// close(fdIn);
		// fdIn = fileno(inFile);
	} else {
		bodyFileFd = open(this->body.c_str(), O_RDONLY);
	}
	int pid = fork();
	if (pid < 0) {
		printError("fork() error");
	}
	if (pid == 0) {
		dup2(fdOut, STDOUT_FILENO);
		if (this->bodySource == FD) {
			dup2(bodyFileFd, STDIN_FILENO);
			close(bodyFileFd);
		} else {
			dup2(fdIn, STDIN_FILENO);
			close(fdIn);
		}
		close(fdOut);
		execve(argv[0], argv, env);
	}
	// waitpid(pid, 0, 0);
	if (this->bodySource == FD) {
		close(bodyFileFd);
	} else {
		close(fdIn);
	}
	char buf[10000];
	int count = 1;
	while (count > 0) {
		memset(buf, 0, 10000);
		count = read(fdOut, buf, 10000);
		this->respBody += buf;
	}
	close(fdOut);
	this->status = COMPLETED;
}
