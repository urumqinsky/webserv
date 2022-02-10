#include "libraryHeaders.hpp"
#include "Request.hpp"

std::string recv_msg(int fd, int size)
{

	char buf[size + 1];
	bzero(buf, size + 1);
	if (recv(fd, buf, size, 0) == -1)
		printError("recv() error");
	std::string str = std::string(buf);
	return str;
}

void	deleteEvent(int kq, int i, struct kevent *eventList)
{
	struct kevent evSet;
	s_udata *tmp = static_cast<s_udata*>(eventList[i].udata);

	std::cout << "Disconnect " << eventList[i].ident << std::endl;
	EV_SET(&evSet, eventList[i].ident, eventList[i].filter, EV_DELETE, 0, 0, NULL);

	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		return (printError("kevent() error disconnect"));
	if (tmp->socketStatus == 0)
		tmp->socketStatus = 1;
	else
	{
		close(eventList[i].ident);
		delete tmp->ipPort;
		delete tmp->req;
		delete tmp;
	}
}

void	readFromClientSocket(int kq, int i, struct kevent *eventList)
{
	if (eventList[i].flags & EV_EOF)
		deleteEvent(kq, i, eventList);
	std::string msg = recv_msg(eventList[i].ident, (int)eventList[i].data);
	s_udata *tmp = static_cast<s_udata*>(eventList[i].udata);
	// std::cout << msg << "\n";
	tmp->req->parseFd(msg);
}

void	writeToClientSocket(int kq, int i, struct kevent *eventList)
{
	s_udata *tmp = static_cast<s_udata*>(eventList[i].udata);
	if (eventList[i].flags & EV_EOF)
		deleteEvent(kq, i, eventList);
	else if (tmp->req->getStatus() == TO_WRITE)
	{
		
		ssize_t sended = send(eventList[i].ident, tmp->req->getResponce().c_str(),
			tmp->req->getResponce().length(), 0); // buf -> req->getResponce
		if (sended == -1)
			printError("send() error");

		tmp->req->makeRequestDefault();
	}
	else if (tmp->req->getStatus() == COMPLETED)
	{
		tmp->req->setStatus(TO_WRITE);
	}
}

void	printError(const std::string &comment)
{
	std::cerr << comment << '\n';
}
