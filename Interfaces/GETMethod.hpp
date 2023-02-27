#ifndef GET_METHOD
#define GET_METHOD
// #include "../parsing/parsing.hpp"
# include "ServerConfiguration.hpp"
# include "Client.hpp"
# define SOCKET int

class GETMethod{
	public:
		void			callGET( ClientInfo *, ServerConfiguration & );
		void			handleGETMethod(ClientInfo *, ServerConfiguration &);
		void			redirectToWebsite(void);
		std::string 	format_date(time_t t);
		std::string 	get_file_type(mode_t mode);
		std::string		directoryListing(std::string , std::string, ClientInfo *);
		std::string		CGIexecutedFile( std::string , ClientInfo *, ServerConfiguration & );
		std::string		bodyFromCgiHeader(char *);
};

#endif