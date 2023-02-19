#ifndef GET_METHOD
#define GET_METHOD
#include "../parsing/parsing.hpp"
# include "ServerConfiguration.hpp"
# include "Client.hpp"
# define SOCKET int

class GETMethod{
	public:
		bool		callGET( ClientInfo *, ServerConfiguration &, std::list<ClientInfo *>::iterator & );
		std::string	handleGETMethod(std::map<std::string, std::string> &, ServerConfiguration &);
		void		redirectToWebsite(void);
		int			directoryListing(char *, SOCKET );
};

#endif