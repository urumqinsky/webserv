#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "libraryHeaders.hpp"

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

	std::string const &getIPport();
	void setIPport(int IPport);
	Status getStatus();

	void parseFd(std::string req);
	friend void parseStartLine(Request &other);
	friend void parseHeader(Request &other);

protected:
	std::string method;
	std::string path;
	std::string http;


private:
	Request(const Request &other);
	Request	&operator=(const Request &other);

	std::string IPport; //?
	std::string body;
	std::string buf;
	int errorStatus;


	Status	status;	
	chunkStatus	chunkStatus;

	std::map <std::string, std::string> headers;
	std::string responce;  

};

#endif