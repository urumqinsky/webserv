#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>

struct generalContext
{
	std::vector<std::string>	index;
	size_t						BodySizeMax;
	std::string					root;
	bool						autoindex;
	std::map<int, std::string>	error_page;
};

struct locationContex
{
	std::vector<std::string>			lockArgs;
	std::vector<std::string>			methods;
	std::string							cgiPath;
	std::string							cgiExtension;
	std::vector<struct locationContext>	lockListL;
	std::string							alias;
	struct generalContext				genL;
};

struct serverContext
{
	std::string						ip;
	int								port;
	std::string						server_name;
	std::vector<locationContext>	lockListS;
	struct generalContext			genS;

};

struct httpContext
{
	std::vector<serverContext>	serverList;
	struct generalContext		genH;
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
	const ServerConfig	&operator=(const ServerConfig &other);
	void				parseConfigFile(const char *filename);

private:
	ServerConfig();
	httpContext					httpCont;
	std::vector<listenIpPort>	listenIpPorts;
};

#endif
