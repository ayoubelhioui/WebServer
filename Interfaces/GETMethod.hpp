#ifndef GET_METHOD
#define GET_METHOD
// #include "../parsing/parsing.hpp"
# include "ServerConfiguration.hpp"
# include "Client.hpp"
# define SOCKET int

class GETMethod{
	public:
		std::string		callGET( ClientInfo *, ServerConfiguration & );
		std::string		handleGETMethod(ParsingRequest &, ServerConfiguration &);
		void			redirectToWebsite(void);
		std::string 	format_date(time_t t);
		std::string 	get_file_type(mode_t mode);
		std::string		directoryListing(std::string , std::string);
		std::string		CGIexecutedFile( std::string , std::string, ServerConfiguration & );
		std::string		bodyFromCgiHeader(char *);
};

#endif