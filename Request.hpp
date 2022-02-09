#ifndef REQUEST_HPP
# define REQUEST_HPP

class Request;

#include "libraryHeaders.hpp"
#include "ServerConfig.hpp"
#include "functions.hpp"

#include <cstring>

std::string provaideDate();
std::string readFromFile(std::string file);
std::string createStatusLine(int code, std::map<int, std::string> &m);
std::string ifAlias(locCont *locConf, std::string path);
std::string getHeader(std::string token, std::map<std::string, std::string> &headers);
template<typename T>
std::string integerToString(T number);
std::string clearFromSlash(std::string str);

enum Status {
	START_LINE,
	HEADERS,
	HEAD_END,
	BODY,
	CHUNKED_BODY,
	COMPLETED,
	TO_WRITE,
	ERROR,
	MOVE_ON

};

enum chunkStatus {
	NUM,
	TEXT,
	END
};

enum bodySource {
	STR,
	FD
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
	void setStatus(Status status);
	std::string getResponce();
	void setAllErrorCodes();
	void parseFd(std::string req);
	friend void parseStartLine(Request &other);
	friend void parseHeader(Request &other);
	friend void parseBody(Request &other);
	friend void parseChunkedBody(Request &other);
	friend void writeToFile(Request &other, std::string str);
	friend void	writeToClientSocket(int i, struct kevent *eventList);

	void createResponce();
	void createBody();
	void createErrorBody();
	friend bool checkRequest(Request &other);
	friend std::string searchIndexFile(Request &other);
	friend void autoindexOn(Request &other);
	friend locCont *findLocation(Request &other);
	friend serCont *findServer(Request &other);
	// void checkSetAlias(locCont *locConf);
	bool checkIfCgi();
	void cgiHandler();

	void getPostMethod ();
	// void postMethod ();
	// void putMethod ();
	void deleteMethod ();
	
	void setClientIpPort(const lIpPort &other);
	void makeRequestDefault();

	std::string getContentLenght();
	std::string getContentType();
	std::string getQueryString();
	std::string getScriptName();
	std::string getClientIp();
	std::string getClientPort();
	std::string getPathInfo();

protected:
	static unsigned long long requestNumber;
	std::string method;
	std::string path;
	std::string http;
	std::string aliasPath; //w/o root
	std::string fullPath; //with root
	std::string pathConfCheck;
	std::map <std::string, std::string> headers;
	std::string cookie;

	htCont *conf;
	locCont *locConf;
	std::string ip;
	int port;
	lIpPort clientIpPort;

	std::string requestLine;
	std::string scriptName;

private:
	Request(const Request &other);
	Request	&operator=(const Request &other);

	std::string buf;
	int errorCode;


	Status	status;
	std::string body;
	chunkStatus	chunkStatus;
	bodySource bodySource;

	size_t tmpBodySize;
	size_t chunkedBodySize;

	std::string responce;
	std::string respBody;

	std::string serverName;
	// int respCode;

	std::map<int, std::string> allErrorCodes;

};

#endif
