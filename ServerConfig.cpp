#include "ServerConfig.hpp"

ServerConfig::ServerConfig(const char *str)
{
	parseConfigFile(str);
	initListenIpPorts();
}

ServerConfig::~ServerConfig()
{}

ServerConfig::ServerConfig(const ServerConfig &other)
{
	*this = other;
}

const ServerConfig	&ServerConfig::operator=(const ServerConfig &other)
{
	if (this != &other)
	{
		httpCont = other.httpCont;
		listenIpPorts = other.listenIpPorts;
	}
	return (*this);
}

void	ServerConfig::checkGeneralContent(genCont &to, genCont &from)
{
	if (to.autoindex == 0 && from.autoindex != 0)
		to.autoindex = from.autoindex;
	if (to.bodySizeMax == (size_t)0 && from.bodySizeMax != (size_t)0)
		to.bodySizeMax = from.bodySizeMax;
	if (to.error_page.size() == (size_t)0 && from.error_page.size() != (size_t)0)
		to.error_page = from.error_page;
	if (to.index.size() == (size_t)0 && from.index.size() != (size_t)0)
		to.index = from.index;
	if (to.root.size() == (size_t)0 && from.root.size() != (size_t)0)
		to.root = from.root;
	if (to.index.size() == (size_t)0)
		to.index.push_back("index.html");
	if (from.index.size() == (size_t)0)
		from.index.push_back("index.html");
}

void	ServerConfig::inheritenceHandler()
{
	if (httpCont.genH.error_page.empty())
		httpCont.genH.error_page.insert(std::pair<int, std::string>(404, "/web/errors/404.html"));
	for (size_t i = 0; i < httpCont.serverList.size(); i++)
	{
		checkGeneralContent(httpCont.serverList[i].genS, httpCont.genH);
		for (size_t j = 0; j < httpCont.serverList[i].locListS.size(); j++)
		{
			checkGeneralContent(httpCont.serverList[i].locListS[j].genL,
									httpCont.serverList[i].genS);
		}
	}
}

void	ServerConfig::parseConfigFile(const char *filename)
{
	std::ifstream				in(filename);
	std::string					line;
	std::vector<std::string>	splitted;

	if (in.is_open())
	{
		while (getline(in, line))
		{
			splitted = split(line, " ");
			if (splitted.back().back() == ';')
				splitted.back().erase(splitted.back().size() - 1);
			if (!(line.compare("")))
				continue ;
			else if (!(splitted[0].compare("server")))
				httpCont.serverList.push_back(parseServer(in, line));
			else if (parseGeneral(splitted, httpCont.genH))
				;
			else
			{
				std::cerr << "Invalid configuration file" << std::endl;
				exit(1);
			}
		}
	}
	inheritenceHandler();
}

void	ServerConfig::parseErrorPage(std::vector<std::string> splitted,
										std::map<int, std::string> &err_map)
{
	std::string	path = splitted.back();

	for (size_t i = 1; i < splitted.size() - 1; i++)
		err_map[std::atoi(splitted[i].c_str())] = path;
}

bool	ServerConfig::parseGeneral(std::vector<std::string> splitted, genCont &gen)
{
	if (!(splitted[0].compare("index")))
	{
		for (size_t i = 1; i < splitted.size(); i++)
			gen.index.push_back(splitted[i]);
		return (true);
	}
	else if (!(splitted[0].compare("max_body_size")))
	{
		gen.bodySizeMax = (size_t)std::atoi(splitted[1].c_str());
		return (true);
	}
	else if (!(splitted[0].compare("autoindex")))
	{
		gen.autoindex = (!(splitted[1].compare("on"))) ? 1 : 2;
		return (true);
	}
	else if (!(splitted[0].compare("root")))
	{
		gen.root = splitted[1];
		return (true);
	}
	else if (!(splitted[0].compare("error_page")))
	{
		parseErrorPage(splitted, gen.error_page);
		return (true);
	}
	return (false);
}

serCont	ServerConfig::parseServer(std::ifstream &in, std::string &line)
{
	std::vector<std::string>	splitted;
	serCont						newServ;

	if (in.is_open())
	{
		while (getline(in, line))
		{
			splitted = split(line, " ");
			if (splitted.back().back() == ';')
				splitted.back().erase(splitted.back().size() - 1);
			if (!(line.compare("")))
				continue ;
			else if (!(splitted[0].compare("}")))
				return (newServ);
			else if (parseGeneral(splitted, newServ.genS))
				;
			else if (!(splitted[0].compare("ip")))
				newServ.ip = splitted[1];
			else if (!(splitted[0].compare("port")))
				newServ.port = std::atoi(splitted[1].c_str());
			else if (!(splitted[0].compare("server_name")))
				newServ.server_name = splitted[1];
			else if (!(splitted[0].compare("location")))
				newServ.locListS.push_back(parseLocation(in, line, splitted));
			else
			{
				std::cerr << "Invalid configuration file" << std::endl;
				exit(1);
			}
		}
	}
	return (newServ);
}

locCont	ServerConfig::parseLocation(std::ifstream &in, std::string &line,
										std::vector<std::string> splitted)
{
	locCont	newLoc;

	for (size_t i = 1; i < splitted.size() - 1; i++)
		newLoc.locArgs.push_back(splitted[i]);
	if (in.is_open())
	{
		while (getline(in, line))
		{
			splitted = split(line, " ");
			if (splitted.back().back() == ';')
				splitted.back().erase(splitted.back().size() - 1);
			if (!(line.compare("")))
				continue ;
			else if (!(splitted[0].compare("}")))
				return (newLoc);
			else if (parseGeneral(splitted, newLoc.genL))
				;
			else if (!(splitted[0].compare("allow")))
			{
				for (size_t i = 1; i < splitted.size(); i++)
					newLoc.methods.push_back(splitted[i]);
			}
			else if (!(splitted[0].compare("cgi")))
				newLoc.cgiPath = splitted[1];
			else if (!(splitted[0].compare("cgi_extension")))
				newLoc.cgiExtension = splitted[1];
			else if (!(splitted[0].compare("alias")))
				newLoc.alias = splitted[1];
			else if (!(splitted[0].compare("location")))
			{
				locCont lc = parseLocation(in, line, splitted);
				newLoc.locListL.push_back(lc);
			}
			else if (!(splitted[0].compare("authorization")))
			{
				newLoc.authorization = splitted[1];
				if (newLoc.authorization == "on")
				{
					newLoc.users.insert(std::pair<std::string, std::string>("ltuluneltulune", "oiwerhashasljfpaosf"));
					newLoc.users.insert(std::pair<std::string, std::string>("rlandorlando", "ldskfttdlkjsakjlkas"));
					newLoc.users.insert(std::pair<std::string, std::string>("heulaheula", "aeirjdlfknnbtsdsadf"));
				}
			}
			else
			{
				std::cerr << "Invalid configuration file" << std::endl;
				exit(1);
			}
		}
	}

	return (newLoc);
}

std::vector<lIpPort>	ServerConfig::getListenIpPorts() const
{
	return (listenIpPorts);
}

htCont	ServerConfig::getHttpCont() const
{
	return (httpCont);
}

std::vector<std::string>	ServerConfig::split(std::string s, std::string delimiter)
{
	size_t	pos_start = 0;
	size_t	pos_end;
	size_t	delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	std::replace(s.begin(), s.end(), '\t', ' ');
	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		if (token.compare(""))
			res.push_back(token);
	}
	res.push_back(s.substr(pos_start));
	return res;
}

void	ServerConfig::initListenIpPorts()
{
	lIpPort		tmp;

	for (size_t i = 0; i < httpCont.serverList.size(); i++)
	{
		tmp.ip = httpCont.serverList[i].ip;
		tmp.port = httpCont.serverList[i].port;
		listenIpPorts.push_back(tmp);
	}
	std::vector<lIpPort>::iterator end = listenIpPorts.end();
	for (std::vector<lIpPort>::iterator it = listenIpPorts.begin(); it != end; ++it) {
		end = std::remove(it + 1, end, *it);
	}
	listenIpPorts.erase(end, listenIpPorts.end());
}

lIpPort::lIpPort()
{
	ip = "";
	port = 0;
}

bool	lIpPort::operator==(const lIpPort &other)
{
	return (this->ip == other.ip && this->port == other.port);
}

genCont::genCont()
{
	this->autoindex = 0;
	this->bodySizeMax = 0;
	this->root = "";
}

serCont::serCont()
{
	this->ip = "";
	this->port = 0;
	this->server_name = "";
}
