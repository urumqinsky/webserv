#ifndef REQUEST_HPP
# define REQUEST_HPP

class Request;

#include "libraryHeaders.hpp"
#include "ServerConfig.hpp"
#include "functions.hpp"

#include <cstring>

enum Status {
	START_LINE,
	HEADERS,
	BODY,
	CHUNKED_BODY,
	COMPLETED,
	ERROR
};

enum chunkStatus {
	NUM,
	TEXT
};

class Request {
public:
	Request(std::string Server);//to discuss
	Request();
	~Request();


	Request(htCont *conf, lIpPort *ip);
	std::string const &getipPort();
	void setipPort(int ipPort);
	Status getStatus();
	std::string getResponce();

	void parseFd(std::string req);
	friend void parseStartLine(Request &other);
	friend void parseHeader(Request &other);
	friend void parseBody(Request &other);
	friend void	writeToClientSocket(int i, struct kevent *eventList);

	void createResponce();
	friend void checkRequest(Request &other);
protected:
	std::string method;
	std::string path;
	std::string http;
	std::map <std::string, std::string> headers;


private:
	Request(const Request &other);
	Request	&operator=(const Request &other);

	htCont *conf;

	std::string ip;
	int port;
	std::string body;
	std::string buf;
	int errorStatus;


	Status	status;
	chunkStatus	chunkStatus;

	std::string responce;
	int respCode;

};

#endif
