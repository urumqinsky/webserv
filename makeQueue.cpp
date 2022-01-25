#include "ServerConfig.hpp"
#include "ServerSocket.hpp"
#include "Request.hpp"
#include "functions.hpp"

bool	compareWithListenSockets(unsigned long ident, ServerSocket *sSockets, int nPorts)
{
	for (int i = 0; i < nPorts; i++)
	{
		if ((unsigned int)sSockets[i].getSocketFd() == ident)
			return (true);
	}
	return (false);
}

void	disconnectSocket(int kq, int i, struct kevent *eventList)
{
	struct kevent evSet;

	std::cout << "Disconnect " << eventList[i].ident << std::endl;
	EV_SET(&evSet, eventList[i].ident, eventList[i].filter, EV_DELETE, 0, 0, NULL);

	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		return (printError("kevent() error disconnect"));
	// close(eventList[i].ident); //закрыть сокет только после удаления второго события
	// если оно есть (1-ое READ, 2-ое WRITE)
}

void	acceptNewClient(int kq, int i, struct kevent *eventList)
{
	int					newEventFd;
	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);
	struct kevent		evSet;

	newEventFd = accept(eventList[i].ident, (struct sockaddr*) &addr, &addrLen);
	if (newEventFd == -1)
		return (printError("accept() error"));
	s_udata *tmp = new s_udata;
	tmp->req = NULL;
	tmp->ipPort = static_cast<lIpPort*>(eventList[i].udata);
	EV_SET(&evSet, newEventFd, EVFILT_READ, EV_ADD, 0, 0, tmp);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		return (printError("kevent() error"));
}

void	watch_loop(int kq, ServerSocket *sSockets, int nPorts, htCont conf)
{
	struct kevent		eventList[1024];
	int					eventNumber;

	while (true)
	{
		eventNumber = kevent(kq, NULL, 0, eventList, 1024, NULL);
		if (eventNumber < 1)
			return (printError("kevent() error"));
		for (int i = 0; i < eventNumber; i++)
		{
			if (eventList[i].flags & EV_EOF)
				disconnectSocket(kq, i, eventList);
			else if (compareWithListenSockets(eventList[i].ident, sSockets, nPorts))
				acceptNewClient(kq, i, eventList);
			else if (eventList[i].filter == EVFILT_READ)
				readFromClientSocket(kq, i, eventList, new htCont(conf));
			else if (eventList[i].filter == EVFILT_WRITE)
				writeToClientSocket(i, eventList);
		}
	}
}

void	makeQueue(ServerSocket *servSockets, int nPorts, ServerConfig &sConfig)
{
	int	kq;
	struct kevent	changeList[nPorts];

	(void)sConfig;
	kq = kqueue();
	if (kq == -1)
		return (printError("kqueue() error"));
	for (int i = 0; i < nPorts; i++)
	{
		lIpPort *tmp = new lIpPort(servSockets[i].getIpPort());
		EV_SET(&changeList[i], servSockets[i].getSocketFd(),
				EVFILT_READ, EV_ADD, 0, 0, static_cast<void*>(tmp));
	}
	if (kevent(kq, changeList, nPorts, NULL, 0, NULL) == -1)
		return (printError("kevent() error"));
	watch_loop(kq, servSockets, nPorts, sConfig.getHttpCont());
}
