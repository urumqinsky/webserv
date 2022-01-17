#include "ServerConfig.hpp"
#include "headers.hpp"

ServerConfig::ServerConfig(const char *str)
{
	if (str == nullptr)
		throw std::exception();
	parseConfigFile(str);
}

ServerConfig::~ServerConfig()
{

}

ServerConfig::ServerConfig(const ServerConfig &other)
{

}

const ServerConfig &ServerConfig::operator=(const ServerConfig &other)
{

}

void ServerConfig::parseConfigFile(const char *filename)
{
	std::ifstream in(filename);
	std::string line;

	if (in.is_open())
	{
		while (getline(in, line))
		{
			if (checkGeneral(httpCont.genH, line))
				;
			else if (line.find("server", 0, 6))
				httpCont.serverList.push_back(parseServer(in, line));
			else
			{
				std::cerr << "Invalid configuration file" << std::endl;
				exit(1);
			}
		}
	}
}
