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
	size_t bytes_read = recv(fd, buf, sizeof(buf), 0);
	cout << "REQUEST from fd = " << fd << "\n" << buf << endl;
}

void watch_loop(int kq, int ls, int ls2)
{
	struct kevent evSet;
	struct kevent evList[2];
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
			else if (evList[i].ident == ls2)
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
	struct sockaddr_in	addr, addr2;
	int					ls, ls2, ok, ok2, kq;
	struct kevent		evSet, evSet2;
	socklen_t			socklen;
	int					opt = 1;
	ls = socket(AF_INET, SOCK_STREAM, 0);
	ls2 = socket(AF_INET, SOCK_STREAM, 0);
	if (ls == -1 || ls2 == -1)
	{
		cerr << "socket error" << endl;
		exit(1);
	}
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(ls2, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr2.sin_family = AF_INET;
	addr.sin_port = htons(8082); // nomer porta > 1023
	addr2.sin_port = htons(8083);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //ip address
	addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
	// socklen = sizeof(addr);
	ok = bind(ls, (struct sockaddr*)&addr, sizeof(addr));
	ok2 = bind(ls2, (struct sockaddr*)&addr2, sizeof(addr2));
	if (ok == -1 || ok2 == -1)
	{
		perror("couldn't bind to socket");
		// cerr << "bind error" << endl;
		exit(1);
	}
	if (listen(ls, 16) == -1 || listen(ls2, 16))
	{
		cerr << "listen error" << endl;
		exit(1);
	}
	kq = kqueue();
	EV_SET(&evSet, ls, EVFILT_READ, EV_ADD, 0, 0, NULL);
	EV_SET(&evSet2, ls2, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1
		|| kevent(kq, &evSet2, 1, NULL, 0, NULL) == -1)
	{
		cerr << "kevent error" << endl;
		exit(1);
	}
	 watch_loop(kq, ls, ls2);
}
//http://eradman.com/posts/kqueue-tcp.html
//https://github.com/frevib/kqueue-tcp-server/blob/master/tcpserver_kqueue.c
