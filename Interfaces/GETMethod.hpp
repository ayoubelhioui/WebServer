#ifndef GET_METHOD
#define GET_METHOD
// #include "../parsing/parsing.hpp"
# include "ServerConfiguration.hpp"
# include "Client.hpp"
# define SOCKET int

class GETMethod{
	public:
		bool			callGET( ClientInfo *, ServerConfiguration &, std::list<ClientInfo *>::iterator & );
		std::string		handleGETMethod(ParsingRequest &, ServerConfiguration &);
		void			redirectToWebsite(void);
		std::string 	format_date(time_t t);
		std::string 	get_file_type(mode_t mode);
		void			directoryListing(std::string , std::string );
		std::string		CGIexecutedFile( std::string , std::string, ServerConfiguration & );
};

#endif