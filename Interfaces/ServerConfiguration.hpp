#ifndef __SERVERCONFIGURATION_H__
# define __SERVERCONFIGURATION_H__

#define PAGE_NOT_FOUND_STATUS "404"
#define MISSING_SERVER_NAME "Provide server name"
#define INVALID_PORT_MSG "Port must contain numbers only."
#define LISTEN_ERROR_MSG "Error on listen keyword"
#define ERROR_PAGE_ERROR_MSG "Error on error_page keyword"
#define SERVER_KEYWORD "server"
#define LISTEN_KEYWORD "listen"
#define SERVER_NAME_KEYWORD "server_name"
#define ERROR_PAGE_KEYWORD "error_page"
#define MAX_CLIENT_BODY_SIZE_KEYWORD "max_client_body_size"
#define INVALID_KEYWORD " : Is An Invalid Keyword."
#define CONFIG_FILE_COULDNT_OPEN_MSG "Couldn't Open Config File."
#define SERVER_KEYWORD_MSG "Error on server keyword"
#define ERROR_PAGE_FILE_NOT_FOUND "Error_Page file is not exist !!"
#define SOCKET  int
#define MAX_REQUEST_SIZE 2000
# include <fstream>
# include "LocationBlockParse.hpp"
class ServerConfiguration {
    public:
        ServerConfiguration();
		// ServerConfiguration	&operator= ( const ServerConfiguration & );
        std::string serverHost;
        std::string serverPort;
        unsigned int clientBodyLimit;
        bool isClosed;
        std::list<std::string> serverName;
        std::list<std::pair<int, std::string> > errorInfo;
        std::list<LocationBlockParse> Locations;
        void fillingDataFirstPart(std::string &);
        void listenKeywordFound(std::vector<std::string> &);
        void serverNameKeywordFound(std::vector<std::string> &);
        void errorPageKeywordFound(std::vector<std::string> &);
        void clientBodySizeKeywordFound(std::vector<std::string> &);
		void	printServerConfiguration ( void );
};

#endif