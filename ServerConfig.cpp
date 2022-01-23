#include "ServerConfig.hpp"

ServerConfig::ServerConfig(const char *str)
{
	std::memset(&httpCont, 0, sizeof(htCont));
	if (str == nullptr)
		throw std::exception();
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

void	ServerConfig::inheritenceHandler()
{

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

void	parseErrorPage(std::vector<std::string> splitted, std::map<int, std::string> &err_map)
{
	// std::string	path = splitted.back();

	// for (size_t i = 0; i < splitted.size() - 1; i++)
	// 	err_map[std::atoi(splitted[i].c_str())] = path;
	// for (std::map<int, std::string>::iterator it = err_map.begin(); it != err_map.end(); ++it)
	// 	std::cout << it->first << " " << it->second << '\n';
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
	// else if (!(splitted[0].compare("error_page")))
	// 	parseErrorPage(splitted, gen.error_page);
	// {
	// 	for (size_t i = 1; i < splitted.size(); i++)
	// 		gen.error_page.push_back(splitted[i]);
	// 	return (true);
	// }
	return (false);
}

serCont	ServerConfig::parseServer(std::ifstream &in, std::string &line)
{
	std::vector<std::string>	splitted;
	serCont						newServ;

	std::memset(&newServ, 0, sizeof(serCont));
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

locCont	ServerConfig::parseLocation(std::ifstream &in, std::string &line, std::vector<std::string> splitted)
{
	locCont	newLoc;

	std::memset(&newLoc, 0, sizeof(locCont));
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

htCont						ServerConfig::getHttpCont() const
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

void						ServerConfig::initListenIpPorts()
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

bool lIpPort::operator==(const lIpPort &other)
{
	return (this->ip == other.ip && this->port == other.port);
}

void						ServerConfig::inheritenceHandler()
{

}
