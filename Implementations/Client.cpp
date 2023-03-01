#include "../Interfaces/Client.hpp"
	
ClientInfo::ClientInfo( void ) : isFirstRead(true) , addressLength(sizeof(this->address)), inReadCgiOut(0), isErrorOccured(false), isServing(false)
, stillWaiting(0), isFirstCgiRead(0), PostFinishedCgi(0)
{
}

ClientInfo::~ClientInfo( void )
{
}		


ClientInfo	&ClientInfo::operator= ( const ClientInfo &obj )
{
	(void)obj;
	// if (this == &obj)
	// 	return (*this);
	// return (*this);
	return (*this);
}

ClientInfo::ClientInfo ( const ClientInfo &obj )
{
	(void)obj;
	// std::cout << " Yes i got called\n";
	// *this = obj;
}

std::string    ClientInfo::generateRandString ( void )
{
    std::string    randString;
    int            n;

    std::srand(std::time(0));
    const std::string    chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    n = rand() % 8 + 5;
    for (int i = 0; i < n; ++i) {
        randString += chars[(rand()) % chars.size()];
    }
    return randString;
}

void	ClientInfo::parsingCgiLine(std::string line){
	std::stringstream str(line);
    std::string word;
    str >> word;
    std::string save = word;
    std::string last;
    while (str >> word)
    {
        last += " ";
        last += word;
    }
    last.erase(0, 1);
    cgiMap[save] = last;
}

void	ClientInfo::parseCgiHeader(std::string &header){
	std::string	headerPart(header), line;
    std::size_t found = headerPart.find("\r\n");
    while(found != std::string::npos)
    {
      line = headerPart.substr(0, found);
      parsingCgiLine(line);
      headerPart = headerPart.substr(found + 2);
      found = headerPart.find("\r\n");
    }
	headerPart = headerPart.substr(found + 1);
    found = headerPart.find("\r\n");
    line = headerPart.substr(0, found);
    parsingCgiLine(line);
}
std::string		ClientInfo::CGIexecutedFile( std::string php_file, ClientInfo *client, ServerConfiguration &server){
    int     pid = 0;
    const char * request_method = client->parsedRequest.requestDataMap["method"].c_str(); // POST or GET
    const char * script_name = "CGIS/php-cgi"; // php cgi inside location
	const char * query_string = client->parsedRequest.queryString.length() == 0 ? "" : client->parsedRequest.queryString.c_str();
    const char * server_host = server.serverHost.c_str();
    const char * server_port = server.serverPort.c_str();
    //const char * path_info ;
    //const char * content_length;
    // const char *content_type;

    // PATH INFO
    setenv("REQUEST_METHOD", request_method, 1);
    setenv("QUERY_STRING", query_string, 1);
    setenv("SCRIPT_NAME", script_name, 1);
    setenv("SERVER_NAME", server_host, 1)   ;
    setenv("SERVER_PORT", server_port, 1)   ;
    setenv("REDIRECT_STATUS", "200", 1)    ;
    // setenv("PATH_INFO", server_port, 1)   ;
    //  setenv("CONTENT_LENGTH", server_port, 1)   ;
    //  setenv("CONTENT_TYPE", server_port, 1)   ;
    int fd[2];
    pipe(fd);
    pid = fork();
    if (pid == 0){
        char  *args[3];
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        args[0] = (char *) script_name;
        args[1] = (char *) php_file.c_str();
        args[2] = NULL;
        if (execve(script_name, args, NULL)){
            std::cout << "exec problem" << std::endl;
            return "";
        }
    }
    close(fd[1]);
    client->CgiReadEnd = fd[0];
    client->inReadCgiOut = 1;
    client->stillWaiting = 1;
    client->cgiPid = pid;
    return "";
}

// void	ClientInfo::clients_Setup()
// {
// 	ClientInfo::_maxSocket = 0;
// 	FD_ZERO(&reads);
//     FD_SET(server_socket, &reads);
//     FD_ZERO(&writes);
//     FD_SET(server_socket, &writes);
//     _maxSocket = std::max(_maxSocket, server_socket);
//     std::list<ClientInfo *>::iterator client_data_it1 = client_data.begin();
//     for (; client_data_it1 != client_data.end(); client_data_it1++)
//     {
//         FD_SET((*client_data_it1)->socket, &reads);
//         FD_SET((*client_data_it1)->socket, &writes);
//         std::max(_maxSocket, (*client_data_it1)->socket); // ????
//     }
//     if( select(_maxSocket + 1, &reads, &writes, NULL, NULL) == -1){
// 		errorPrinting("select has failed");
// 	}
// }

// void	ClientInfo::checkingClientListenning(int server_socket, std::list<ClientInfo *> &client_data
// , fd_set &reads, fd_set &writes){
// 	if (FD_ISSET(server_socket, &reads))
//     {
//         ClientInfo *client = ClientInfo::get_client(-1, client_data);
//         client->socket = accept(server_socket, (struct sockaddr *) &(client->address), &(client->address_length));
// //           fcntl(client->socket, F_SETFL, O_NONBLOCK);
//         FD_SET(client->socket, &reads);
//         FD_SET(client->socket, &writes);
//         _maxSocket = std::max(_maxSocket, client->socket);
//         if (client->socket < 0) std::cerr << "accept function failed\n";
//     }
// }

// ClientInfo *ClientInfo::get_client(int socket, std::list<ClientInfo *> &data_list)
// {
//     std::list<ClientInfo *> copy = data_list;
//     std::list<ClientInfo *>::iterator it = copy.begin();
//     for(; it != copy.end(); it++)
//         if(socket == (*it)->socket) return *it;
//     ClientInfo *new_node = new ClientInfo;
//     data_list.push_front(new_node);
//     return new_node;
// }
