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
void    ClientInfo::CGIexecutedFile( ClientInfo *client, ServerConfiguration &server )
{
    int     pid = 0;
    
    const char * request_method = client->parsedRequest.requestDataMap["method"].c_str(); // POST or GET
    const char * script_name = client->cgiIterator->second.c_str(); // php cgi inside location
	const char * query_string = client->parsedRequest.queryString.length() == 0 ? "" : client->parsedRequest.queryString.c_str();
    const char * server_host = server.serverHost.c_str();
    const char * server_port = server.serverPort.c_str();
    //const char * path_info ;
    std::cout << "request_method is " << request_method << std::endl;
    std::cout << "query_string is " << query_string << std::endl;
    std::cout << "server_host is " << server_host << std::endl;
    std::cout << "server_post is " << server_port << std::endl;
    const char * content_length = client->cgiContentLength.c_str();
    const char *content_type = client->cgiContentType.c_str();
    std::cout << "content_length is " << content_length << std::endl;
    std::cout << "content_type is " << content_type << std::endl;
    setenv("REQUEST_METHOD", request_method, 1);
    setenv("QUERY_STRING", query_string, 1);
    setenv("SCRIPT_NAME", script_name, 1);
    setenv("SERVER_NAME", server_host, 1)   ;
    setenv("SERVER_PORT", server_port, 1)   ;
    setenv("REDIRECT_STATUS", "200", 1)    ;
    // setenv("PATH_INFO", server_port, 1)   ;
      setenv("CONTENT_LENGTH", content_length, 1)   ;
      setenv("CONTENT_TYPE", content_type, 1)   ;
    int fd[2];
    pipe(fd);
    pid = fork();
    if (pid == 0){
        char  *args[3];
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        args[0] = (char *) script_name;
        args[1] = (char *) client->servedFileName.c_str();
        args[2] = NULL;
        if (execve(script_name, args, NULL) == -1){
            exit(1);
        }
    }
    close(fd[1]);
    client->CgiReadEnd = fd[0];
    client->inReadCgiOut = 1;
    client->stillWaiting = 1;
    client->cgiPid = pid;
}
