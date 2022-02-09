#include "ServerSocket.hpp"
#include "ServerConfig.hpp"
#include "functions.hpp"
#include "Request.hpp"

int		main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cout << "Invalid input arguments" << std::endl;
		return (1);
	}
	if (argv[1] == nullptr || access(argv[1], F_OK) == -1) {
		std::cout << "Invalid argument" << std::endl;
		return (1);
	}
	ServerConfig	sConfig(argv[1]);
	// printConfig(sConfig);
	int nPorts = sConfig.getListenIpPorts().size();
	std::vector<lIpPort> listenIpPorts = sConfig.getListenIpPorts();
	ServerSocket	servSockets[nPorts];
	for (int i = 0; i < nPorts; i++) {
		try {
			servSockets[i].setSocketForListen(*((listenIpPorts[i].ip).c_str()),
													listenIpPorts[i].port);
		}
		catch(const std::runtime_error & e) {
			std::cerr << e.what() << '\n';
			return (1);
		}
	}
	makeQueue(servSockets, nPorts, sConfig);
	return (0);
}

