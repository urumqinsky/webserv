#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

int main(int argc, char *argv[])
{
	std::string filename = std::string(std::getenv("PWD")) + "/" + std::string(argv[1]);
	std::ifstream in(filename.c_str());
	std::string line;
	bool op;
	if((op = in.is_open()))
	{
		while (getline(in, line))
			std::cout << line << std::endl;
	}
	std::cout << op << std::endl;
	return 0;
}