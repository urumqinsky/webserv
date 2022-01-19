#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "headers.hpp"

#include <cstring>

enum Status {
	FIRST_LINE,
	HEADERS,
	BODY,
	chunkED_BODY,
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
	void parseFd(std::string req);

private:
	Request(const Request &other);
	Request	&operator=(const Request &other);

	std::string IPport; //?
	std::string method;
	std::string path;
	std::string body;
	std::string buf;
	int errorStatus;


	Status	status;	
	chunkStatus	chunkStatus;

	std::map <std::string, std::string> headers;
	std::string responce;  

};

#endif