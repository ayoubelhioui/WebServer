# include "../webserver.hpp"

std::string	generateRandString ( void )
{
    std::string	randString;
	int			n;

	std::srand(std::time(0));
    const std::string	chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
		"0123456789";
	n = rand() % 8 + 5;
    for (int i = 0; i < n; ++i) {
        randString += chars[(rand()) % chars.size()];
    }
    return randString;
}