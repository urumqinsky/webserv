#include "ServerSocket.hpp"
#include "ServerConfig.hpp"

void	makeQueue(ServerSocket *servSockets, int nPorst);

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

int		main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cout << "Invalid input arguments" << std::endl;
		exit(1);
	}
	ServerConfig	sConfig(argv[1]);

	// printConfig(sConfig);
	int nPorts = sConfig.getListenIpPorts().size();
	std::vector<lIpPort> listenIpPorts = sConfig.getListenIpPorts();

	// for (size_t i = 0; i < listenIpPorts.size(); i++)
	// 	std::cout << "ip:port = " << listenIpPorts[i].ip << ":" << listenIpPorts[i].port << '\n';
	ServerSocket	servSockets[nPorts];

	for (int i = 0; i < nPorts; i++)
	{
		try
		{
			servSockets[i].setSocketForListen(*((listenIpPorts[i].ip).c_str()), listenIpPorts[i].port);
		}
		catch(const std::runtime_error & e)
		{
			std::cerr << e.what() << '\n';
			return (1);
		}
	}
	makeQueue(servSockets, nPorts);
	return (0);
}

void recv_msg(int fd)
{
	char buf[1024];
	bzero(buf, 1024);
	recv(fd, buf, sizeof(buf), 0);
	std::cout << "REQUEST from fd = " << fd << "\n" << buf << std::endl;
}

bool	compareWithListenSockets(unsigned long ident, ServerSocket *sSockets, int nPorts)
{
	for (int i = 0; i < nPorts; i++)
	{
		if ((unsigned int)sSockets[i].getSocketFd() == ident)
			return (true);
	}
	return (false);
}

void	watch_loop(int kq, ServerSocket *sSockets, int nPorts)
{
	struct kevent		evSet;
	struct kevent		eventList[1024];
	int					eventNumber, newEventFd;
	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);

	while (1)
	{
		eventNumber = kevent(kq, NULL, 0, eventList, 1024, NULL);
		if (eventNumber < 1)
		{
			std::cerr << "kevent() error" << std::endl;
			return ;
		}
		for (int i = 0; i < eventNumber; i++)
		{
			if (eventList[i].flags & EV_EOF)
			{
				std::cout << "Disconnect " << eventList[i].ident << std::endl;
				EV_SET(&evSet, eventList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "kevent() error" << std::endl;
					return ;
				}
				close(eventList[i].ident);
			}
			else if (compareWithListenSockets(eventList[i].ident, sSockets, nPorts))
			{
				newEventFd = accept(eventList[i].ident, (struct sockaddr*) &addr, &addrLen);
				if (newEventFd == -1)
				{
					std::cerr << "accept() error" << std::endl;
					return ;
				}
				EV_SET(&evSet, newEventFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "kevent() error" << std::endl;
					return ;
				}
			}
			else if (eventList[i].filter == EVFILT_READ)
			{
				recv_msg(eventList[i].ident); //read from socket
				//считать с сокета запрос от клиента
				//обработать запрос
				EV_SET(&evSet, eventList[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
				{
					std::cerr << "kevent() error" << std::endl;
					return ;
				}
			}
			else if (eventList[i].filter == EVFILT_WRITE)
			{
				send(eventList[i].ident, (const void*)"hello from server\n", 18, 0);
				//бесконечная отправка
				//нужно разобраться с udata
			}
		}
	}
}

void	makeQueue(ServerSocket *servSockets, int nPorts)
{
	int	kq;
	struct kevent	changeList[nPorts];

	kq = kqueue();
	if (kq == -1)
	{
		std::cerr << "kqueue() error" << std::endl;
		return ;
	}
	for (int i = 0; i < nPorts; i++)
	{
		EV_SET(&changeList[i], servSockets[i].getSocketFd(),
				EVFILT_READ, EV_ADD, 0, 0, NULL);
	}
	if (kevent(kq, changeList, nPorts, NULL, 0, NULL) == -1)
	{
		std::cerr << "kevent() error" << std::endl;
		return ;
	}
	watch_loop(kq, servSockets, nPorts);
}
