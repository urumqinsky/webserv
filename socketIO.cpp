#include "libraryHeaders.hpp"
#include "Request.hpp"

std::string recv_msg(int fd, int size)
{

	char buf[size];
	bzero(buf, size);
	recv(fd, buf, size, 0);
	std::string str = std::string(buf);
	return str;
}

void	readFromClientSocket(int kq, int i, struct kevent *eventList, htCont *conf)
{
	// struct kevent evSet;
	(void)kq;
	std::string msg = recv_msg(eventList[i].ident, (int)eventList[i].data); //read from socket
	//считать с сокета запрос от клиента
	//обработать запрос
	s_udata *tmp = static_cast<s_udata*>(eventList[i].udata);
	if (tmp->req == NULL)
	{
		tmp->req = new Request(conf);
		// EV_SET(&evSet, eventList[i].ident, EVFILT_READ, EV_ADD, 0, 0, static_cast<void*>(tmp));
		// if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		// 	return (printError("kevent() error"));
	}
	tmp->req->parseFd(msg);

	// if (req->getStatus() == COMPLETED)
	// {
	// 	EV_SET(&evSet, eventList[i].ident, EVFILT_WRITE, EV_ADD, 0, 0, (void*)req);
	// 	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
	// 		return (printError("kevent() error"));
	// }

}

void	writeToClientSocket(int i, struct kevent *eventList)
{
	s_udata *tmp = static_cast<s_udata*>(eventList[i].udata);
	// Request *req = (Request*)(eventList[i].udata);
	if (tmp->req->getStatus() == COMPLETED) {
		std::ifstream fs("/Users/heula/webserv/level1.html");
		std::string line;
		std::string buf = "HTTP/1.1 200 OK\r\nServer: webserv\r\nContent-Type: text/html\r\n\r\n";
		while (getline(fs, line))
			buf += line + "\n";

		send(eventList[i].ident, buf.c_str(), buf.length(), 0); // buf -> req->getResponce
		// send(eventList[i].ident, req->getResponce().c_str(), req->getResponce().length(), 0); // buf -> req->getResponce
		tmp->req->status = START_LINE;
	}
	//бесконечная отправка
	//нужно разобраться с udata в struct kevent
	//отправлять нужно только готовый ответ
}

void	printError(const std::string &comment)
{
	std::cerr << comment << '\n';
}
