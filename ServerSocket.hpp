#include "headers.hpp"

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

class ServerSocket
{
public:
	ServerSocket();
	ServerSocket(const char &ip, int port);
	~ServerSocket();
	ServerSocket(const ServerSocket &other);
	ServerSocket	&operator=(const ServerSocket &other);
	int				getSocketFd() const;
	void			setSocketForListen(const char &ip, int port);
private:
	int					listenSocketFd;
	struct sockaddr_in	listenSocketAddr;
};

#endif
