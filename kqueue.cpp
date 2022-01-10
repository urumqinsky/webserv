#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>
#include <sys/event.h>

using namespace std;

int main(int argc, char *argv[])
{
	struct sockaddr_in	addr;
	int					ls, ok, kq;
	struct kevent		evSet;

	ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls == -1)
		cerr << "socket error" << endl;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7654); // nomer porta > 1023
	addr.sin_addr.s_addr = inet_addr("127.0.0"); //ip address
	ok = bind(ls, (struct sockaddr*) &addr, sizeof(addr));
	if (ok == -1)
		cerr << "bind error" << endl;
	if (listen(ls, 16) == -1)
		cerr << "listen error" << endl;
	kq = kqueue();
	EV_SET(&evSet, ls, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		cerr << "kevent error" << endl;
	//  watch_loop(kq);
}
//http://eradman.com/posts/kqueue-tcp.html
//https://github.com/frevib/kqueue-tcp-server/blob/master/tcpserver_kqueue.c
