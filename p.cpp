#include "webserver.hpp"
int main()
{
	std::ifstream a("./file.conf");
	a.close();
	std::ifstream b("./Makefile");
	b.close();
	int fd = open("./getFormat.cpp", O_RDONLY);
	std::cout << "fd " << fd << std::endl;
}