#include "ServerConfig.hpp"
#include "ServerSocket.hpp"

#ifndef FUNCTIONHEADER_HPP
# define FUNCTIONHEADER_HPP

void	printConfig(ServerConfig &sc);
void	makeQueue(ServerSocket *servSockets, int nPorst, ServerConfig &sConfig);


#endif
