#ifndef GET_METHOD
#define GET_METHOD
#include "../parsing/parsing.hpp"
# include "ServerConfiguration.hpp"
# include "Client.hpp"
# define SOCKET int

class GETMethod{
	public:
		void		callGET( ClientInfo *, ServerConfiguration & );
		std::string	handleGETMethod(std::map<std::string, std::string> &, ServerConfiguration &);
		void		redirectToWebsite(void);
		int			directoryListing(char *, SOCKET );
};

#endif