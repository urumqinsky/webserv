#include "ServerConfig.hpp"
#include "ServerSocket.hpp"

#ifndef FUNCTIONHEADER_HPP
# define FUNCTIONHEADER_HPP

void	printConfig(ServerConfig &sc);
void	printError(const std::string &comment);
void	makeQueue(ServerSocket *servSockets, int nPorst, ServerConfig &sConfig);
void	writeToClientSocket(int i, struct kevent *eventList);
void	readFromClientSocket(int kq, int i, struct kevent *eventList, htCont *conf);

#endif
