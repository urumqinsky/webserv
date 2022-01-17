#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>

struct generalContext
{
	std::vector<std::string>	index;
	size_t						BodySizeMax;
	std::string					root;
	bool						autoindex;
};

struct locationContex
{
	std::vector<std::string>			lockArgs;
	std::vector<std::string>			allowedMethods;
	std::string							cgiPath;
	std::string							cgiExtension;
	std::vector<struct locationContext>	lockListL;
	std::string							alias;
	struct generalContext				generalL;
};

struct serverContext
{
	std::string						ip;
	int								port;
	std::string						server_name;
	std::vector<locationContext>	lockListS;
	struct generalContext			generalS;

};

struct httpContext
{
	std::vector<serverContext>	serverList;
	std::vector<std::string>	error_pages;
	struct generalContext		generalH;

};

struct listenIpPort
{
	std::string	ip;
	int			port;
};

class ServerConfig
{
public:
	ServerConfig(const char *str);
	~ServerConfig();
	ServerConfig(const ServerConfig &other);
	const ServerConfig &operator=(const ServerConfig &other);
	void parseConfigFile(const char *str);


private:
	ServerConfig();
	httpContext	httpCont;
	std::vector<listenIpPort> listenIpPorts;
};

#endif
