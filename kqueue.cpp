#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>
#include <sys/event.h>
#include <cstdlib>
#include <unistd.h>

using namespace std;

void recv_msg(int fd)
{
	char buf[1024];
	bzero(buf, 1024);
	cout << "test" << endl;
	size_t bytes_read = recv(fd, buf, sizeof(buf), 0);
	cout << "REQUEST from fd = " << fd << "\n" << buf << endl;
}

void watch_loop(int kq, int ls)
{
	struct kevent evSet;
	struct kevent evList[32];
	int nev, i;
	struct sockaddr_in addr;
	socklen_t socklen = sizeof(addr);
	int fd;

	while (1)
	{
		nev = kevent(kq, NULL, 0, evList, 32, NULL);
		if (nev < 1)
		{
			cerr << "kevent error" << endl;
			exit(1);
		}
		for (i = 0; i < nev; i++)
		{
			if (evList[i].flags & EV_EOF)
			{
				cout << "disconnect" << evList[i].ident << endl;
				fd = evList[i].ident;
				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
				{
					cerr << "kevent error" << endl;
					exit(1);
				}
				close(fd);
			}
			else if (evList[i].ident == ls)
			{
				fd = accept(evList[i].ident, (struct sockaddr*) &addr, &socklen);
				if (fd == -1)
				{
					cerr << "accept error" << endl;
					exit(1);
				}
				EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL); // check evList is full?
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
				{
					cerr << "kevent error" << endl;
					exit(1);
				}
			}
			else if (evList[i].filter == EVFILT_READ)
			{
				recv_msg(evList[i].ident); //read from socket
				send(evList[i].ident, (const void*)"Hello from server\n", 18, 0);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	struct sockaddr_in	addr;
	int					ls, ok, kq;
	struct kevent		evSet;
	socklen_t			socklen;
	int					opt = 1;
	ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls == -1)
	{
		cerr << "socket error" << endl;
		exit(1);
	}
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8081); // nomer porta > 1023
	addr.sin_addr.s_addr = INADDR_ANY; //ip address
	cout << "s_addr = " << (int)(addr.sin_addr.s_addr) << endl;
	socklen = sizeof(addr);
	ok = bind(ls, (struct sockaddr*)&addr, sizeof(addr));
	cout << "sockname = " << getsockname(ls, (sockaddr*)&addr, &socklen) << endl;
	if (ok == -1)
	{
		cerr << "bind error" << endl;
		exit(1);
	}
	if (listen(ls, 16) == -1)
	{
		cerr << "listen error" << endl;
		exit(1);
	}
	kq = kqueue();
	EV_SET(&evSet, ls, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
	{
		cerr << "kevent error" << endl;
		exit(1);
	}
	 watch_loop(kq, ls);
}
//http://eradman.com/posts/kqueue-tcp.html
//https://github.com/frevib/kqueue-tcp-server/blob/master/tcpserver_kqueue.c
