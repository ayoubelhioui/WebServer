#ifndef GET_METHOD
#define GET_METHOD
#include "../parsing/parsing.hpp"
# include "Client.hpp"
class GETMethod{
	public:
		void		callGET(ClientInfo &);
		std::string	handleGETMethod(std::map<std::string, std::string> &, Parsing &);
		void		redirectToWebsite(void);
		int			directoryListing(char *);
};

#endif