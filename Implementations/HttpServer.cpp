# include "../webserver.hpp"
# include "../Interfaces/HttpServer.hpp"
# include "../Interfaces/ChunkedPostRequest.hpp"
#   include <algorithm>
//# define RECEIVING_FINISHED (received, contentLength) (received == contentLength) ? (true) : (false);
// # include "../parsing/parsing.hpp"
// //std::cout
/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

bool    reverseSort(LocationBlockParse &a, LocationBlockParse &b){
    return (a.Location > b.Location);

}
HttpServer::HttpServer ( ServerConfiguration &serverConfig )
	: _serverConfiguration(serverConfig) 
{
	_serverConfiguration.Locations.sort(reverseSort);
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
	// //std::cout << "_serverConfig host is " << _serverConfiguration.serverHost << std::endl;
	getaddrinfo(_serverConfiguration.serverHost.c_str(),
				_serverConfiguration.serverPort.c_str(),
				&_serverHints, &bindAddress);
	
	_listeningSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (_listeningSocket < 0)
		exit (EXIT_FAILURE); // to be replaced by sth else
	// //std::cout << "Socket Created Successfully" << std::endl;
	optval = 1;
    setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (bind(_listeningSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
		exit (EXIT_FAILURE); // to be replaced by sth else
	// //std::cout << "Binded Successfully" << std::endl;
	freeaddrinfo(bindAddress);
	if (listen(_listeningSocket, MAXQUEUESIZE) < 0)
		exit (EXIT_FAILURE); // to be replaced by sth else
	// //std::cout << "Set Up for LISTENING Successfully" << std::endl;
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
//		 fcntl(newClient->socket, F_SETFL, O_NONBLOCK); // need to be understood.
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
		if ((FD_ISSET((*ClientInfoIt)->socket, &(this->_readFds)) && (*ClientInfoIt)->isErrorOccured == false
        && (*ClientInfoIt)->isServing == false) || (*ClientInfoIt)->inReadCgiOut == true)
		{
			if ((*ClientInfoIt)->isFirstRead)
			{
				(*ClientInfoIt)->parsedRequest.receiveFirstTime((*ClientInfoIt)->socket);
				(*ClientInfoIt)->parsedRequest.parse();
				(*ClientInfoIt)->parseQueryString();
				if(isUriTooLong((*ClientInfoIt)->parsedRequest.requestDataMap["path"]))
					error_414(*ClientInfoIt);
				if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET")
				{
					GETMethod getRequest;
					try
					{
						getRequest.callGET(*ClientInfoIt, this->_serverConfiguration);
						if((*ClientInfoIt)->cgiIterator != (*ClientInfoIt)->_currentLocation->CGI.end())
                        {
                            (*ClientInfoIt)->cgiContentType = "";
							(*ClientInfoIt)->cgiContentLength = "0";
							(*ClientInfoIt)->CGIexecutedFile((*ClientInfoIt), this->_serverConfiguration);
						}
					}
					catch(std::exception &e)
					{
						std::cout << "callGet exception : " << e.what() << std::endl;
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
                    (*ClientInfoIt)->tempPathForLocation = (*ClientInfoIt)->parsedRequest.requestDataMap["path"];
					(*ClientInfoIt)->returnPathWithoutFile((*ClientInfoIt)->tempPathForLocation);
                    (*ClientInfoIt)->checkPathValidation(*ClientInfoIt, this->_serverConfiguration, (*ClientInfoIt)->tempPathForLocation);
					if ((*ClientInfoIt)->parsedRequest.requestDataMap["Transfer-Encoding:"] == "chunked")
					{
						(*ClientInfoIt)->chunkedRequest = new ChunkedPostRequest;
						(*ClientInfoIt)->chunkedRequest->handleFirstRecv(((*ClientInfoIt)->parsedRequest.requestDataMap["Content-Type:"]).c_str()
														, (*ClientInfoIt)->parsedRequest);
					}
					else
					{
						(*ClientInfoIt)->postRequest = new PostMethod(this->_serverConfiguration);
					 	try
					 	{
							 (*ClientInfoIt)->postRequest->handleFirstRead(*ClientInfoIt);
							 if ((*ClientInfoIt)->parsedRequest.received == (*ClientInfoIt)->parsedRequest.contentLength)
								(*ClientInfoIt)->postRequest->preparingMovingTempFile(*ClientInfoIt);
					 	}
					 	catch (std::exception &e)
					 	{
							 (*ClientInfoIt)->isErrorOccured = true;
							 std::cout << e.what() << std::endl;
					 	}
					}
				}
				(*ClientInfoIt)->isFirstRead = false;
			}
			else
			{
				if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "POST" and
                    (*ClientInfoIt)->parsedRequest.received < (*ClientInfoIt)->parsedRequest.contentLength
                    and ((*ClientInfoIt)->isErrorOccured == false) and ((*ClientInfoIt)->isServing == false)
					and ((*ClientInfoIt)->inReadCgiOut == false))
                {
					try
                    {
						(*ClientInfoIt)->postRequest->receiveTheBody(*ClientInfoIt);
					}
                    catch (std::exception &e)
                    {
						error_500(*ClientInfoIt);
						std::cout << e.what() << std::endl;
                        (*ClientInfoIt)->isErrorOccured = true;
                    }
                }
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["Transfer-Encoding:"] == "chunked")
				{
					(*ClientInfoIt)->chunkedRequest->handleRecv((*ClientInfoIt)->socket);
					if ((*ClientInfoIt)->chunkedRequest->uploadDone == true)
					{
						this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
						continue;
					}
					
				}
				else if ((*ClientInfoIt)->inReadCgiOut)
				{
					if((*ClientInfoIt)->stillWaiting)
					{
						int retWait = waitpid((*ClientInfoIt)->cgiPid, NULL, WNOHANG);
						if(retWait == 0)
						{
							ClientInfoIt++;
							continue;
						}
						else if (retWait == (*ClientInfoIt)->cgiPid)
						{
							(*ClientInfoIt)->stillWaiting = 0;
							(*ClientInfoIt)->isFirstCgiRead = 1;
						}
					}
					else
					{
						if((*ClientInfoIt)->isFirstCgiRead)
						{
         					char buffer[1001];
         					ssize_t n;
         					n = read((*ClientInfoIt)->CgiReadEnd, buffer, 1000);
         					buffer[n] = 0;
         					std::string str_buffer(buffer);
         					int bef_header = (*ClientInfoIt)->parsedRequest.retIndex(buffer);
         					 std::string header_part = str_buffer.substr(0, bef_header);
                            (*ClientInfoIt)->parseCgiHeader(header_part);
                             std::string contentType = (*ClientInfoIt)->cgiMap["Content-type:"];
                            std::string mimeType = contentType.substr(0, contentType.find(";"));
         					std::string body = str_buffer.substr(bef_header + 4);
							std::string newFile = "FilesForServing/" + (*ClientInfoIt)->generateRandString() + get_real_format(mimeType.c_str());
							if((*ClientInfoIt)->cgi_out.is_open()) (*ClientInfoIt)->cgi_out.close();
							(*ClientInfoIt)->cgi_out.open(newFile, std::ios::binary);
							(*ClientInfoIt)->servedFileName = newFile; // case of no upload /testcmd.php or the case of uploaded file example.php
							(*ClientInfoIt)->cgi_out << body;
							(*ClientInfoIt)->isFirstCgiRead = false;

						}
						else
						{
							char buffer[1001];
							ssize_t n;
							n = read((*ClientInfoIt)->CgiReadEnd , buffer, 1000);
        					buffer[n] = 0;
                            (*ClientInfoIt)->cgi_out << buffer;
                            if(n < 1000)
							{
                                (*ClientInfoIt)->cgi_out.seekp(0, std::ios::end);
                                (*ClientInfoIt)->served_size = (*ClientInfoIt)->cgi_out.tellp();
                                (*ClientInfoIt)->cgi_out.seekp(0, std::ios::beg);
                                std::string headerPart = "HTTP/1.1 200 OK\r\n"
								    + std::string("Connection: close\r\n")
								    + std::string("Content-Length: ")
								    + std::to_string((*ClientInfoIt)->served_size)
								    + "\r\n"
								    + std::string("Content-Type: ")
								    + get_mime_format((*ClientInfoIt)->servedFileName.c_str())
								    + "\r\n\r\n";
								std::cout << "" << std::endl;
								send((*ClientInfoIt)->socket, headerPart.c_str(), headerPart.length(), 0);
								close((*ClientInfoIt)->CgiReadEnd);
								(*ClientInfoIt)->inReadCgiOut = false;
								(*ClientInfoIt)->PostFinishedCgi = true;
								if((*ClientInfoIt)->served.is_open()) (*ClientInfoIt)->served.close();
								(*ClientInfoIt)->served.open((*ClientInfoIt)->servedFileName, std::ios::binary);
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
						and (*ClientInfoIt)->isErrorOccured == false and (*ClientInfoIt)->isServing == false
						and (*ClientInfoIt)->inReadCgiOut == false and (*ClientInfoIt)->PostFinishedCgi == false) 
					{
						try 
						{
							(*ClientInfoIt)->postRequest->writeToUploadedFile(*ClientInfoIt);
							if ((*ClientInfoIt)->postRequest->totalTempFileSize == 0)
							{
								(*ClientInfoIt)->postRequest->sourceFile.close();
    							(*ClientInfoIt)->postRequest->destinationFile.close();
								size_t foundPhp = (*ClientInfoIt)->parsedRequest.uploadFileName.find(".php");
								if((*ClientInfoIt)->isNotUpload)
								{
									chmod((*ClientInfoIt)->actionPath.c_str(),  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
									(*ClientInfoIt)->servedFileName = (*ClientInfoIt)->actionPath;
									int acc = access((*ClientInfoIt)->actionPath.c_str(), X_OK | F_OK);
                                    if(acc == -1)
									{
                                        error_404(*ClientInfoIt);
                                        throw std::runtime_error("executed path was not found");
                                    }
									std::ifstream actionPathFile((*ClientInfoIt)->servedFileName);
									actionPathFile.seekg(0, std::ios::end);
    								int file_size = actionPathFile.tellg();
    								actionPathFile.seekg(0, std::ios::beg);
									actionPathFile.close();
									(*ClientInfoIt)->cgiContentType = "text/php";
									(*ClientInfoIt)->cgiContentLength = std::to_string(file_size);
									(*ClientInfoIt)->CGIexecutedFile((*ClientInfoIt), this->_serverConfiguration);
								}
								else if (foundPhp != std::string::npos
								&& (*ClientInfoIt)->cgiIterator != (*ClientInfoIt)->_currentLocation->CGI.end())
								{
									chmod((*ClientInfoIt)->postFilePath.c_str(),  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                                    (*ClientInfoIt)->servedFileName = (*ClientInfoIt)->postFilePath;
                                    int acc = access((*ClientInfoIt)->servedFileName.c_str(), X_OK | F_OK);
                                    if(acc == -1)
									{
                                        error_404(*ClientInfoIt);
                                        throw std::runtime_error("executed path was not found");
                                    }
                                    (*ClientInfoIt)->cgiContentType = "text/php";
									(*ClientInfoIt)->cgiContentLength = std::to_string((*ClientInfoIt)->postRequest->cgiContentLength);
									(*ClientInfoIt)->CGIexecutedFile((*ClientInfoIt), this->_serverConfiguration);
								}
								else
								{
									(*ClientInfoIt)->postRequest->successfulPostRequest(*ClientInfoIt);
									(*ClientInfoIt)->isServing = true;
								}
							}
							// exeuteCGI;
						}
						catch (std::exception &e)
						{
							std::cout << e.what() << std::endl;
							(*ClientInfoIt)->isErrorOccured = true;
						}
					}
					else if ((((*ClientInfoIt)->isErrorOccured == true) or ((*ClientInfoIt)->isServing == true))
							   or
							   ((*ClientInfoIt)->inReadCgiOut == false and (*ClientInfoIt)->PostFinishedCgi == true))
					{
						char *s = new char[1024]();
						(*ClientInfoIt)->served.read(s, 1024);
						int r = (*ClientInfoIt)->served.gcount();
//                        //std::cout << "i will write : " << r << std::endl;
						if (send((*ClientInfoIt)->socket, s, r, 0) == -1)
						{
							this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
							delete[] s;
							continue;
						}
						delete[] s;
						if (r < 1024)
						{
//						    //std::cout << "helloWorld" << std::endl;
							std::list<ClientInfo *>::iterator temp_it = ClientInfoIt;
							// if ((*ClientInfoIt)->currentServerFile != "")
							// 	std::remove((*ClientInfoIt)->currentServerFile.c_str());
							// delete *ClientInfoIt;
							if((*ClientInfoIt)->_currentLocation->UploadDirectoryPath.length())
								(*ClientInfoIt)->_currentLocation->UploadDirectoryPath = "";
							close((*ClientInfoIt)->socket);
							(*ClientInfoIt)->served.close();
							ClientInfoIt++;
							this->_clientsList.erase(temp_it);
							continue;
						}
					}
				} 
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET")
				{
					if ((*ClientInfoIt)->inReadCgiOut == false) 
					{
						char *s = new char[1025]();
						(*ClientInfoIt)->served.read(s, 1024);
						int r = (*ClientInfoIt)->served.gcount();
						if (send((*ClientInfoIt)->socket, s, r, 0) == -1) {
							this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
							continue;
						}
						delete[] s;
						if (r < 1024) 
						{
							this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
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
	this->_setUpListeningSocket();
}