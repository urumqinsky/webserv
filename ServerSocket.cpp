#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : listenSocketFd(-1)
{
}

ServerSocket::ServerSocket(const char &ip, int port)
{
	setSocketForListen(ip, port);
}

ServerSocket::~ServerSocket()
{
	close(listenSocketFd);
}

ServerSocket::ServerSocket(const ServerSocket &other)
{
	*this = other;
}

ServerSocket	&ServerSocket::operator=(const ServerSocket &other)
{
	if (this != &other)
	{
		this->listenSocketFd = other.listenSocketFd;
		this->listenSocketAddr = other.listenSocketAddr;
	}
	return (*this);
}

int				ServerSocket::getSocketFd() const
{
	return (listenSocketFd);
}

void			ServerSocket::setSocketForListen(const char &ip, int port)
{
	ipPort.ip = std::string(&ip);
	ipPort.port = port;
	listenSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocketFd == -1)
		throw std::runtime_error("socket() error");
	memset(&listenSocketAddr, 0, sizeof(listenSocketAddr));
	int opt = 1;
	setsockopt(listenSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	listenSocketAddr.sin_family = AF_INET;
	listenSocketAddr.sin_port = htons(port);
	listenSocketAddr.sin_addr.s_addr = inet_addr(&ip);
	opt = bind(listenSocketFd, (struct sockaddr*)&listenSocketAddr,
				sizeof(listenSocketAddr));
	if (opt == -1)
	{
		close(listenSocketFd);
		throw std::runtime_error("bind() error");
	}
	if ((listen(listenSocketFd, 16)) == -1)
	{
		close(listenSocketFd);
		throw std::runtime_error("listen() error");
	}
}

lIpPort			ServerSocket::getIpPort() const
{
	return (ipPort);
}
