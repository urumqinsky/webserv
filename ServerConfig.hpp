#include "libraryHeaders.hpp"
#include "Request.hpp"

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

struct	genCont
{
	std::vector<std::string>	index; // files, to find in
	size_t						bodySizeMax;
	std::string					root;
	int							autoindex; // 1 - on , 2 - off , 0 - without autoindex
	std::map<int, std::string>	error_page;
	genCont();
};

struct	locCont
{
	std::vector<std::string>			locArgs; // path.begin() file
	std::vector<std::string>			methods; // method
	std::string							cgiPath;
	std::string							cgiExtension;
	std::vector<struct locCont>			locListL;
	std::string							alias;
	std::string							authorization;
	std::map<std::string, std::string>	users;
	genCont						genL;
};

struct	serCont
{
	std::string				ip; //IPport
	int						port;
	std::string				server_name; //Host
	std::vector<locCont>	locListS;
	genCont					genS;
	serCont();
};

struct	htCont
{
	std::vector<serCont>	serverList; // [ipport + servername]
	genCont					genH;
};

struct	lIpPort
{
	std::string	ip;
	int			port;
	lIpPort();
	bool operator==(const lIpPort &other);
};

struct	s_udata
{
	lIpPort	*ipPort;
	Request	*req;
	int		socketStatus;
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
	locCont						parseLocation(std::ifstream &in, std::string &line,
												std::vector<std::string> splitted);
	void						parseErrorPage(std::vector<std::string> splitted, std::map<int,
												std::string> &err_map);
	std::vector<std::string>	split(std::string s, std::string delimiter);
	void						initListenIpPorts();
	void						inheritenceHandler();
	void						checkGeneralContent(genCont &to, genCont &from);
};

#endif
