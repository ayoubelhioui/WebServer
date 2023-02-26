# include "../webserver.hpp"
# include "../Interfaces/HttpServer.hpp"
// # include "../parsing/parsing.hpp"
// std::cout
/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

HttpServer::HttpServer ( ServerConfiguration &serverConfig )
	: _serverConfiguration(serverConfig) 
{
	 }

HttpServer::HttpServer ( void ) { }
HttpServer::~HttpServer ( void ) { }

HttpServer	&HttpServer::operator= ( const HttpServer &obj )
{
	if (this == &obj)
		return (*this);
	this->_serverConfiguration = obj._serverConfiguration;
	return (*this);
}

HttpServer::HttpServer ( const HttpServer &obj )
{
	*this = obj;
}
/*
// */

/* ----------------------------------------------------- */
/* ------------------ MEMBER FUNCTIONS ----------------- */
/* ----------------------------------------------------- */

void	HttpServer::_setUpListeningSocket( void )
{
	struct addrinfo *bindAddress;
	int				optval; // ???

	memset(&_serverHints, 0, sizeof(_serverHints));
	_serverHints.ai_family = AF_INET;
	_serverHints.ai_socktype = SOCK_STREAM;
	_serverHints.ai_flags = AI_PASSIVE;
	// std::cout << "_serverConfig host is " << _serverConfiguration.serverHost << std::endl;
	getaddrinfo(_serverConfiguration.serverHost.c_str(),
				_serverConfiguration.serverPort.c_str(),
				&_serverHints, &bindAddress);
	
	_listeningSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (_listeningSocket < 0)
		exit (EXIT_FAILURE); // to be replaced by sth else
	// std::cout << "Socket Created Successfully" << std::endl;
	optval = 1;
    setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (bind(_listeningSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
		exit (EXIT_FAILURE); // to be replaced by sth else
	// std::cout << "Binded Successfully" << std::endl;
	freeaddrinfo(bindAddress);
	if (listen(_listeningSocket, MAXQUEUESIZE) < 0)
		exit (EXIT_FAILURE); // to be replaced by sth else
	// std::cout << "Set Up for LISTENING Successfully" << std::endl;
}

void	HttpServer::_selectClients ( void )
{
	_maxSocket = _listeningSocket;
	std::list<ClientInfo *>::iterator	ClientInfoIt;
	ClientInfoIt = this->_clientsList.begin();
	FD_ZERO(&_readFds);
    FD_SET(this->_listeningSocket, &_readFds);
    FD_ZERO(&_writeFds);
    FD_SET(this->_listeningSocket, &_writeFds);
	for (; ClientInfoIt != this->_clientsList.end(); ClientInfoIt++)
	{
		FD_SET((*ClientInfoIt)->socket, &_readFds);
        FD_SET((*ClientInfoIt)->socket, &_writeFds);
		_maxSocket = std::max(_maxSocket, (*ClientInfoIt)->socket);
	}
	if (select(_maxSocket + 1, &_readFds, &_writeFds, NULL, NULL) == -1)
		errorPrinting("select has failed"); // to be moved
}

void	HttpServer::setClientInfoList ( std::list<ClientInfo> & )
{

}

// void	HttpServer::_addClient ( SOCKET	clientSocket )
// {
// 	ClientInfo	(*ClientInfoIt);

// 	this->_cli
// }

void	HttpServer::_acceptNewConnection( void )
{
	
	if (FD_ISSET(this->_listeningSocket, &(this->_readFds)))
    {
		ClientInfo	*newClient = new ClientInfo;
        newClient->socket = accept(this->_listeningSocket, (struct sockaddr *) &(newClient->address), &(newClient->addressLength));
		// fcntl(client->socket, F_SETFL, O_NONBLOCK); // need to be understood.
        FD_SET(newClient->socket, &(this->_readFds));
        FD_SET(newClient->socket, &(this->_writeFds));
        _maxSocket = std::max(_maxSocket, newClient->socket);
        if (newClient->socket < 0)
			std::cerr << "accept function failed\n";
		this->_clientsList.push_front(newClient);
    }
}

void HttpServer::dropClient( SOCKET &clientSocket, std::list<ClientInfo *>::iterator &ClientInfoIt)
{
    close(clientSocket);
    std::list<ClientInfo *>::iterator tempIterator = ClientInfoIt;
    delete *ClientInfoIt;
	ClientInfoIt++;
    this->_clientsList.erase(tempIterator);
}


void	HttpServer::_serveClients( void )
{
	std::list<ClientInfo *>::iterator	ClientInfoIt;
	ClientInfoIt = this->_clientsList.begin();
	while (ClientInfoIt != this->_clientsList.end())
	{
		if (FD_ISSET((*ClientInfoIt)->socket, &(this->_readFds))
		|| (*ClientInfoIt)->inReadCgiOut)
		{
			if ((*ClientInfoIt)->isFirstRead)
			{
				(*ClientInfoIt)->parsedRequest.receiveFirstTime((*ClientInfoIt)->socket);
				(*ClientInfoIt)->parsedRequest.parse();
//				std::cout << "*************************" << std::endl;
//				std::cout << "req header" << (*ClientInfoIt)->parsedRequest.requestHeader << std::endl;
//				std::cout << "*************************" << std::endl;
				std::string	word = (*ClientInfoIt)->parsedRequest.requestDataMap["path"];
				size_t	foundQuery = word.find('?');
				if(foundQuery != std::string::npos){
					(*ClientInfoIt)->parsedRequest.requestDataMap["path"] = word.substr(0, foundQuery);
					(*ClientInfoIt)->parsedRequest.queryString = word.substr(foundQuery + 1);
				}
				if(isUriTooLong((*ClientInfoIt)->parsedRequest.requestDataMap["path"]))
				{
					error_414(*ClientInfoIt);
				}
				if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET")
				{
					GETMethod getRequest;
					try{
						getRequest.callGET(*ClientInfoIt, this->_serverConfiguration);
					}
					catch(std::exception &e){
						std::cout << e.what() << std::endl;
						error_404((*ClientInfoIt));
						// continue;
					}
				}
				// else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "DELETE")
				// {
				// 	ClientInfoIt++;
				// 	continue ;
				// }
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "POST")
				{
					(*ClientInfoIt)->postRequest = new PostMethod(this->_serverConfiguration);
					 try
					 {
						 (*ClientInfoIt)->postRequest->handleFirstRead(*ClientInfoIt);
                         if ((*ClientInfoIt)->parsedRequest.received == (*ClientInfoIt)->parsedRequest.contentLength)
                             (*ClientInfoIt)->postRequest->preparingMovingTempFile(*ClientInfoIt);
					 }
					 catch (std::exception &e){
						 (*ClientInfoIt)->isErrorOccured = true;
						 std::cout << e.what() << std::endl;
					 }
				 }
				(*ClientInfoIt)->isFirstRead = false;
			}
			else{
				if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "POST" and
                    (*ClientInfoIt)->parsedRequest.received < (*ClientInfoIt)->parsedRequest.contentLength
                    and (*ClientInfoIt)->isErrorOccured == false and (*ClientInfoIt)->isServing == false)
					    (*ClientInfoIt)->postRequest->receiveTheBody(*ClientInfoIt);
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET"
				&& (*ClientInfoIt)->inReadCgiOut){
					if((*ClientInfoIt)->stillWaiting){
						int retWait = waitpid((*ClientInfoIt)->cgiPid, NULL, WNOHANG);
						if(retWait == 0) {
							ClientInfoIt++;
							continue;
						}
						else if (retWait == (*ClientInfoIt)->cgiPid){
							(*ClientInfoIt)->stillWaiting = 0;
							(*ClientInfoIt)->isFirstCgiRead = 1;
						}
					}
					else{
						if((*ClientInfoIt)->isFirstCgiRead){
         					char buffer[1001];
         					ssize_t n;
         					n = read((*ClientInfoIt)->CgiReadEnd, buffer, 1000);
         					buffer[n] = 0;
         					std::string str_buffer(buffer);
         					int bef_header = (*ClientInfoIt)->parsedRequest.retIndex(buffer);
         					// std::string header_part = str_buffer.substr(0, bef_header);
         					// std::stringstream(header_part);
         					std::string body = str_buffer.substr(bef_header + 4);
							(*ClientInfoIt)->cgi_out << body;
							(*ClientInfoIt)->isFirstCgiRead = 0;
						}
						else{
							char buffer[1001];
							ssize_t n;
							n = read((*ClientInfoIt)->CgiReadEnd , buffer, 1000);
        					buffer[n] = 0;
							(*ClientInfoIt)->cgi_out << buffer;
							if(n < 1000) {
								close((*ClientInfoIt)->CgiReadEnd);
								(*ClientInfoIt)->cgi_out.close();
								(*ClientInfoIt)->inReadCgiOut = 0;
								if((*ClientInfoIt)->served.is_open()) (*ClientInfoIt)->served.close();
								(*ClientInfoIt)->served.open((*ClientInfoIt)->servedFileName, std::ios::binary);
								(*ClientInfoIt)->served.seekg(0, std::ios::end);
								(*ClientInfoIt)->served_size = (*ClientInfoIt)->served.tellg();
								(*ClientInfoIt)->served.seekg(0, std::ios::beg);
							}
						}
					}
				}
			}
		}
		else if(FD_ISSET((*ClientInfoIt)->socket, &(this->_writeFds)))
        {
			if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "POST")
			{
				if ((*ClientInfoIt)->parsedRequest.received == (*ClientInfoIt)->parsedRequest.contentLength
                and (*ClientInfoIt)->isErrorOccured == false and (*ClientInfoIt)->isServing == false)
				{
					(*ClientInfoIt)->postRequest->writeToUploadedFile();
					if ((*ClientInfoIt)->postRequest->totalTempFileSize == 0)
					{
						(*ClientInfoIt)->postRequest->successfulPostRequest(*ClientInfoIt);
						continue ;
					}
				}
                else if (((*ClientInfoIt)->isErrorOccured == true) or ((*ClientInfoIt)->isServing == true)){
                    char *s = new char[1024]();
            		(*ClientInfoIt)->served.read(s, 1024);
                        int r = (*ClientInfoIt)->served.gcount();
                        if (send((*ClientInfoIt)->socket, s, r, 0) == -1){
                            this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
                            delete [] s;
                            continue;
                        }
                        delete [] s;
                        if(r < 1024)
                        {
                            close((*ClientInfoIt)->socket);
                            std::list<ClientInfo *>::iterator temp_it = ClientInfoIt;
                            // if ((*ClientInfoIt)->currentServerFile != "")
                            // 	std::remove((*ClientInfoIt)->currentServerFile.c_str());
                            // delete *ClientInfoIt;
                            (*ClientInfoIt)->served.close();
                            ClientInfoIt++;
                            this->_clientsList.erase(temp_it);
                            continue;
            		    }
                }
			}
			else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET")
			{
				if((*ClientInfoIt)->inReadCgiOut == 0){
            		char *s = new char[1024]();
            		(*ClientInfoIt)->served.read(s, 1024);
            		int r = (*ClientInfoIt)->served.gcount();
					if (send((*ClientInfoIt)->socket, s, r, 0) == -1){
						this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
						continue;
					} 
					delete [] s;
            		if(r < 1024){
            		    close((*ClientInfoIt)->socket);
            		    std::list<ClientInfo *>::iterator temp_it = ClientInfoIt;
						// if ((*ClientInfoIt)->currentServerFile != "")
						// 	std::remove((*ClientInfoIt)->currentServerFile.c_str());
						// delete *ClientInfoIt;
						(*ClientInfoIt)->served.close();
            		    ClientInfoIt++;
            		    this->_clientsList.erase(temp_it);
            		    continue;
            		}
				}
			}
        }
		ClientInfoIt++;
	}
}

void	HttpServer::setUpMultiplexing ( void )
{
	this->_selectClients();
	this->_acceptNewConnection();
	this->_serveClients();
}

void	HttpServer::setUpHttpServer( void )
{
	// std::cout << this->_serverConfiguration.serverHost << std::endl;
	// std::cout << this->_serverConfiguration.serverPort << std::endl;
	this->_setUpListeningSocket();
}