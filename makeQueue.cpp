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

void	acceptNewClient(int kq, int i, struct kevent *eventList, htCont *conf)
{
	int					newEventFd;
	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);
	struct kevent		evSet[2];

	newEventFd = accept(eventList[i].ident, (struct sockaddr*) &addr, &addrLen);
	if (newEventFd == -1)
		return (printError("accept() error"));
	int opt = 1;
	setsockopt(newEventFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	s_udata *tmp = new s_udata;
	tmp->socketStatus = 0;
	tmp->ipPort = new lIpPort(*(static_cast<lIpPort*>(eventList[i].udata)));
	tmp->req = new Request(conf, tmp->ipPort);
	EV_SET(&evSet[0], newEventFd, EVFILT_READ, EV_ADD, 0, 0, static_cast<void*>(tmp));
	EV_SET(&evSet[1], newEventFd, EVFILT_WRITE, EV_ADD, 0, 0, static_cast<void*>(tmp));
	if (kevent(kq, evSet, 2, NULL, 0, NULL) == -1)
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
			if (compareWithListenSockets(eventList[i].ident, sSockets, nPorts))
				acceptNewClient(kq, i, eventList, new htCont(conf));
			else if (eventList[i].filter == EVFILT_READ)
				readFromClientSocket(kq, i, eventList);
			else if (eventList[i].filter == EVFILT_WRITE)
				writeToClientSocket(kq, i, eventList);
		}
	}
}

void	makeQueue(ServerSocket *servSockets, int nPorts, ServerConfig &sConfig)
{
	int	kq;
	struct kevent	changeList[nPorts];

	kq = kqueue();
	if (kq == -1)
		return (printError("kqueue() error"));
	for (int i = 0; i < nPorts; i++)
	{
		lIpPort *tmp = new lIpPort(servSockets[i].getIpPort());
		EV_SET(&changeList[i], servSockets[i].getSocketFd(),
				EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, static_cast<void*>(tmp));
	}
	if (kevent(kq, changeList, nPorts, NULL, 0, NULL) == -1)
		return (printError("kevent() error"));
	watch_loop(kq, servSockets, nPorts, sConfig.getHttpCont());
}
