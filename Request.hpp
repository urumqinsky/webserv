#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "headers.hpp"

class Request {
public:
	Request();
	~Request();
	Request(const Request &other);
	Request	&operator=(const Request &other);

	std::string const &getIPport();
	void setIPport(int IPport);
	void parseFd(int fd);

private:
	std::string IPport;
	std::map <std::string, std::string> request;
	std::string responce;  

};

#endif