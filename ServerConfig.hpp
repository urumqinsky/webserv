#include "libraryHeaders.hpp"

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

struct	genCont
{
	std::vector<std::string>	index;
	size_t						bodySizeMax;
	std::string					root;
	bool						autoindex;
	std::vector<std::string>	error_page;
	genCont(){}
};

struct	locCont
{
	std::vector<std::string>	locArgs;
	std::vector<std::string>	methods;
	std::string					cgiPath;
	std::string					cgiExtension;
	std::vector<struct locCont>	locListL;
	std::string					alias;
	genCont						genL;
	locCont(){}
};

struct	serCont
{
	std::string				ip;
	int						port;
	std::string				server_name;
	std::vector<locCont>	locListS;
	genCont					genS;
	serCont(){}
};

struct	htCont
{
	std::vector<serCont>	serverList;
	genCont					genH;
	htCont(){}
};

struct	lIpPort
{
	std::string	ip;
	int			port;
	lIpPort();
	bool operator==(const lIpPort &other);
};

class ServerConfig
{
public:
	ServerConfig(const char *str);
	~ServerConfig();
	ServerConfig(const ServerConfig &other);
	const ServerConfig			&operator=(const ServerConfig &other);
	void						parseConfigFile(const char *filename);
	std::vector<lIpPort> 		getListenIpPorts() const;
	htCont						getHttpCont() const;
private:
	htCont					httpCont;
	std::vector<lIpPort>	listenIpPorts;
	ServerConfig();
	bool						parseGeneral(std::vector<std::string> splitted, genCont &gen);
	serCont						parseServer(std::ifstream &in, std::string &line);
	locCont						parseLocation(std::ifstream &in, std::string &line, std::vector<std::string> splitted);
	std::vector<std::string>	split(std::string s, std::string delimiter);
	void						initListenIpPorts();
};

#endif
