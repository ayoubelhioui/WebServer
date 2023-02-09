#ifndef _PARSING_HPP_
#define _PARSING_HPP_
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
#define CONFIG_FILE_NAME "parsing/file.conf"
#define CONFIG_FILE_COULDNT_OPEN_MSG "Couldn't Open Config File."
#define SERVER_KEYWORD_MSG "Error on server keyword"
#define ERROR_PAGE_FILE_NOT_FOUND "Error_Page file is not exist !!"
#define SOCKET  int
#define MAX_REQUEST_SIZE 2000
#define MAX_ARRAY_SIZE 40000000

struct locationBlock{
    std::list<std::string> indexFiles;
    std::list<std::pair<std::string, std::string> > CGI;
    std::list<std::string> allowedMethods;
    std::string Location;
    std::string Redirection;
    std::string Root;
    std::string UploadDirectoryPath;
    std::string Path_Info;
    std::string answerFileForDirectory;
    bool    isOpen;
    bool    isDirectoryListingOn;
    locationBlock();
};

struct Parsing{
    std::string serverHost;
    std::string serverPort;
    unsigned int clientBodyLimit;
    bool isClosed;
    std::list<std::string> serverName;
    std::list<std::pair<int, std::string> > errorInfo;
    std::list<locationBlock> Locations;
    Parsing();
    void fillingDataFirstPart(std::string &data);
    void listenKeywordFound(std::vector<std::string> &vec);
    void serverNameKeywordFound(std::vector<std::string> &vec);
    void errorPageKeywordFound(std::vector<std::string> &vec);
    void clientBodySizeKeywordFound(std::vector<std::string> &vec);
};

struct client_info {
    socklen_t address_length;
    struct sockaddr_storage address;
    char address_buffer[128];
    SOCKET socket;
    char *request;
    int received;
    client_info() : received(0){ request = new char[MAX_ARRAY_SIZE + 1]();}
    ~client_info(){delete [] request;}
    struct client_info *next;
};

struct postRequestStruct{
    client_info *client;
    std::list<client_info *>::iterator clientDataIterator;
    std::list<client_info *> clientData;
    std::map<std::string, std::string> &requestData;
    Parsing &configFileData;
    postRequestStruct(client_info *client, std::list<client_info *>::iterator clientDataIterator, std::list<client_info *> clientData,
                      std::map<std::string, std::string> &requestData, Parsing &configFileData) : client(client), clientDataIterator(clientDataIterator), clientData(clientData), requestData(requestData), configFileData(configFileData){};
};

const char *get_real_format(const char *mime_type);
const char *get_mime_format(const char *type);
bool    handlingPostRequest(postRequestStruct &postRequest);
void    error_413(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client);
void    error_400(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client);
void    error_501(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client);
bool    isNotValidPostRequest(std::map<std::string, std::string> &requestData);
const char *get_mime_format(const char *type);
bool    getMatchedLocation(std::string &Uri, std::list<Parsing> &configFileData);
bool    isLocationHaveRedirection(std::string &redirection);
bool    isMethodAllowedInLocation(std::string &requestMethod, std::list<std::string> &allowedMethods);
bool    isIndexExistInLocation(locationBlock &location);
bool    isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData);
void    requestBodyTooLong(client_info *client);
bool    isBodySizeBigger(Parsing &servers, int bodySize, client_info *client);
void    printingParsingData(std::list<Parsing> &parsingData);
bool    isValidNumber(std::string &data);
void    errorPrinting(const char *errorMessage);
void    readingData(std::list<Parsing> &parsingData);
void	location_parse(std::list<std::string>::iterator &it, locationBlock &loc);
bool    all_empty(std::string str);
void	set_upload_file(std::vector<std::string> &vec, locationBlock &loc);
void	set_cgi(std::vector<std::string> &vec, locationBlock &loc);
void	set_indexes(std::vector<std::string> &vec, locationBlock &loc);
void	set_root(std::vector<std::string> &vec, locationBlock &loc);
void	set_dirlisting(std::vector<std::string> &vec, locationBlock &loc);
void	fill_redirection(std::vector<std::string> &vec, locationBlock &loc);
void	fill_allow_methods(std::vector<std::string> &vec, locationBlock &loc);
void	get_path(std::vector<std::string> &vec, locationBlock &loc);
void	server_start(std::list<Parsing> &servers);
bool    isNotValidPostRequest(std::map<std::string, std::string> &requestData);
bool    isUriTooLong(std::string &Uri);
bool    handlingPostRequest(postRequestStruct &postRequest);
void	handle_get_method(std::map<std::string, std::string> &request, Parsing &server);
#endif