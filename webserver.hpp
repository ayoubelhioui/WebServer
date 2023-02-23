#ifndef __WEBSERVER_H__
# define __WEBSERVER_H__

// # include "parsing/parsing.hpp"
// # include "includes/postRequest.hpp"#include <set>
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
#include <sys/stat.h>
#include <unistd.h>
#include <netdb.h>
#include <algorithm>
#include <fcntl.h>
#include <cstdio>
#include <dirent.h>
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
#define CONFIG_FILE_NAME "./file.conf"
#define CONFIG_FILE_COULDNT_OPEN_MSG "Couldn't Open Config File."
#define SERVER_KEYWORD_MSG "Error on server keyword"
#define ERROR_PAGE_FILE_NOT_FOUND "Error_Page file is not exist !!"
#define SOCKET  int
#define MAX_REQUEST_SIZE 2000
bool    isUriTooLong(std::string &Uri);
# include "Interfaces/ConfigFileParser.hpp"
# include "Interfaces/HttpServer.hpp"
# include "Interfaces/MultiHttpServer.hpp"
# include "Interfaces/Client.hpp"
# include "Interfaces/GETMethod.hpp"
# include "Interfaces/LocationBlockParse.hpp"
# include "Interfaces/ServerConfiguration.hpp"

#endif