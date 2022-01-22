#include "ServerConfig.hpp"

void printGeneral(genCont &gen)
{
	if (gen.index.size() != 0)
	{
		std::cout << "index ";
		for (size_t i = 0; i < gen.index.size(); i++)
			std::cout << " " << gen.index[i];
		std::cout << '\n';
	}
	if (gen.autoindex == true)
		std::cout << "autoindex " << gen.autoindex << '\n';
	if (gen.bodySizeMax != 0)
		std::cout << "max_body_size " << gen.bodySizeMax << '\n';
	if (gen.root != "")
		std::cout << "root " << gen.root << '\n';
	if (gen.error_page.size() != 0)
	{
		std::cout << "error_page ";
		for (size_t i = 0; i < gen.error_page.size(); i++)
			std::cout << gen.error_page[i] << " ";
		std::cout << '\n';
	}
}

void printLocation(locCont &loc)
{
	if (loc.locArgs.size() != 0)
	{
		std::cout << "location ";
		for (size_t i = 0; i < loc.locArgs.size(); i++)
			std::cout << loc.locArgs[i] << " ";
		std::cout << '\n';
	}
	if (loc.alias != "")
		std::cout << "alias " << loc.alias << '\n';
	if (loc.cgiPath != "")
		std::cout << "cgi " << loc.cgiPath << '\n';
	if (loc.cgiExtension != "")
		std::cout << "cgi_extension " << loc.cgiExtension << '\n';
	if (loc.methods.size() != 0)
	{
		std::cout << "allow ";
		for (size_t i = 0; i < loc.methods.size(); i++)
			std::cout << loc.methods[i] << " ";
		std::cout << '\n';
	}
	printGeneral(loc.genL);
	if (loc.locListL.size() != 0)
	{
		locCont l;
		for (size_t i = 0; i < loc.locListL.size(); i++)
		{
			std::cout << ".................\n" << "location : " << i << '\n';
			l = loc.locListL[i];
			printLocation(l);
		}
	}

}

void printServer(serCont &sc)
{
	printGeneral(sc.genS);
	std::cout << "ip " << sc.ip << '\n';
	std::cout << "port " << sc.port << '\n';
	std::cout << "server_name " << sc.server_name << '\n';
	for (size_t i = 0; i < sc.locListS.size(); i++)
	{
		std::cout << "----------------\n";
		printLocation(sc.locListS[i]);
	}
}

void printConfig(ServerConfig &sc)
{
	htCont hc = sc.getHttpCont();

	std::cout << "http context: " << '\n';
	printGeneral(hc.genH);
	for (size_t i = 0; i < hc.serverList.size(); i++)
	{
		std::cout << "******************\n" << "server: " << i << '\n';
		printServer(hc.serverList[i]);
	}
}
