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
	friend void parseFirstLine(Request &other);


protected:
	std::string buf;
	Status	status;	
	chunkStatus	chunkStatus;
	std::string method;
	std::string path;
	std::string body;


private:
	Request(const Request &other);
	Request	&operator=(const Request &other);

	std::string IPport; //?
	int errorStatus;



	std::map <std::string, std::string> headers;
	std::string responce;  

};

#endif