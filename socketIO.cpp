#include "libraryHeaders.hpp"
#include "Request.hpp"

std::string recv_msg(int fd, int size)
{

	char buf[size + 1];
	bzero(buf, size + 1);
	recv(fd, buf, size, 0);
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
	tmp->req->parseFd(msg);
}

void	writeToClientSocket(int kq, int i, struct kevent *eventList)
{
	if (eventList[i].flags & EV_EOF)
		deleteEvent(kq, i, eventList);
	s_udata *tmp = static_cast<s_udata*>(eventList[i].udata);
	if (tmp->req->getStatus() == TO_WRITE)
	{
		std::ifstream fs("/Users/ltulune/Desktop/webserv/level1.html");
		std::string line;
		std::string buf = "HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-length: 113\r\nContent-Type: text/html\r\n\r\n";
		while (getline(fs, line))
			buf += line + "\n";
		// send(eventList[i].ident, buf.c_str(), buf.length(), 0); // buf -> req->getResponce
		send(eventList[i].ident, tmp->req->getResponce().c_str(), tmp->req->getResponce().length(), 0); // buf -> req->getResponce
		tmp->req->setStatus(START_LINE);
	}
	else if (tmp->req->getStatus() == ERROR)
	{
		std::cout << "REQEST ERROR" << std::endl;
		tmp->req->setStatus(TO_WRITE);
	}
}

void	printError(const std::string &comment)
{
	std::cerr << comment << '\n';
}
