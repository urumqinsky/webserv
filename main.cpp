#include "ServerSocket.hpp"
#include "serverconfig.hpp"

void	makeQueue(ServerSocket *servSockets, int nPorst);

int		main(int argc, char *argv[])
{
	(void)argv;
	(void)argc;
	// if (argc != 2)
	// {
	// 	std::cout << "Invalid input arguments" << std::endl;
	// 	exit(1);
	// }
	// Config file parser
	int nPorts = 2;
	Config	cfgs[nPorts]; // for example

	cfgs[0].ip = "127.0.0.1";
	cfgs[0].port = 8083;
	cfgs[1].ip = "127.0.0.1";
	cfgs[1].port = 9090;

	ServerSocket	servSockets[nPorts];

	for (int i = 0; i < nPorts; i++)
	{
		try
		{
			servSockets[i].setSocketForListen(*(cfgs[i].ip), cfgs[i].port);
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
				std::cout << "size = " << (unsigned int)(eventList[i].data) << std::endl;
				recv_msg(eventList[i].ident); //read from socket
				//считать с сокета запрос от клиента
				
				//обработать запрос 

				send(eventList[i].ident, (const void*)"Hello from server\n", 18, 0);
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
