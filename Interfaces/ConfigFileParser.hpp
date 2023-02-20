#ifndef _ConfigFileParser_H__
#define _ConfigFileParser_H__
#include <set>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <algorithm>
#include <fcntl.h>
#include <cstdio>
#include "LocationBlockParse.hpp"
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
#define DEFAULT_CONFIG_FILE_NAME "file.conf"
#define CONFIG_FILE_COULDNT_OPEN_MSG "Couldn't Open Config File."
#define SERVER_KEYWORD_MSG "Error on server keyword"
#define ERROR_PAGE_FILE_NOT_FOUND "Error_Page file is not exist !!"
#define SOCKET  int
#define MAX_REQUEST_SIZE 2000
# include "ServerConfiguration.hpp"




class ConfigFileParser {
	public:
		ConfigFileParser ( std::string & );
		ConfigFileParser ( void );
		~ConfigFileParser ( void );
		void	parseConfigFile ( void );
        void	printingParsingData( void );
		std::list<ServerConfiguration>	listOfServersConfiguration;
	private:
		std::list<std::string> 					_configFileLines;				
		std::string								_configFilePath;
		void     _startParsingFile( void );
        void     _readingDataFromFile( void );
};


// ConfigFileParser.parseConfigFile();

// serverConfiguration->se

bool    isValidNumber(std::string &);
void    errorPrinting(const char *);
#endif
