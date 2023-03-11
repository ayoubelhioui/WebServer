# include "../webserver.hpp"
# include "../Interfaces/HttpServer.hpp"
# include "../Interfaces/ChunkedPostRequest.hpp"
# include "../Interfaces/DeleteMethod.hpp"
# include <algorithm>
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
	fcntl(_listeningSocket, F_SETFL, O_NONBLOCK);
	if (_listeningSocket < 0)
	{
		std::cout << "socket function failed" << std::endl;
        exit (EXIT_FAILURE); // to be replaced by sth else
	}
	// //std::cout << "Socket Created Successfully" << std::endl;
	optval = 1;
    setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (bind(_listeningSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
	{
		std::cout << "bind function failed" << std::endl;
        exit (EXIT_FAILURE); // to be replaced by sth else
	}
	// //std::cout << "Binded Successfully" << std::endl;
	freeaddrinfo(bindAddress);
	if (listen(_listeningSocket, MAXQUEUESIZE) < 0)
	{
		std::cout << "listen function failed" << std::endl;
        exit (EXIT_FAILURE); // to be replaced by sth else
	}
	// //std::cout << "Set Up for LISTENING Successfully" << std::endl;
}

void	HttpServer::_selectClients ( void )
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 300;
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
	if (select(_maxSocket + 1, &_readFds, &_writeFds, NULL, &tv) == 0)
		throw std::runtime_error("select has failed or may have no clients at the moment");
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
		ClientInfo	*newClient = new ClientInfo(this->_serverConfiguration);
        newClient->socket = accept(this->_listeningSocket, (struct sockaddr *) &(newClient->address), &(newClient->addressLength));
		fcntl(newClient->socket, F_SETFL, O_NONBLOCK);
		if(newClient->socket == -1)
            throw std::runtime_error("accept has blocked and did not accept any new connection");
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
				try
				{
					(*ClientInfoIt)->parsedRequest.receiveFirstTime((*ClientInfoIt)->socket, (*ClientInfoIt)->recvError);
					(*ClientInfoIt)->parsedRequest.parse();
					if ((*ClientInfoIt)->parsedRequest.checkHost(this->_serverConfiguration) == 0)
					{
						error_400(*ClientInfoIt, this->_serverConfiguration.errorInfo["400"]);
						(*ClientInfoIt)->isErrorOccured = true;
						ClientInfoIt++;
						continue;
					}
					if((*ClientInfoIt)->isValidMethod())
                    {
						(*ClientInfoIt)->isDefaultError = false;
                        (*ClientInfoIt)->isErrorOccured = true;
                        error_405((*ClientInfoIt), this->_serverConfiguration.errorInfo["405"]);
                        throw std::runtime_error("Method not allowed");
                    }
					(*ClientInfoIt)->parseQueryString();
					if(isUriTooLong((*ClientInfoIt)->parsedRequest.requestDataMap["path"]))
					{
						(*ClientInfoIt)->isDefaultError = false;
						error_414(*ClientInfoIt, this->_serverConfiguration.errorInfo["414"]);
						(*ClientInfoIt)->isErrorOccured = true;
						ClientInfoIt++;
						continue;
					}
				}
				catch (std::exception &e)
				{
                    std::cout << e.what() << std::endl;
					if((*ClientInfoIt)->recvError == true)
					{
						this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
						continue;
					}
					if ((*ClientInfoIt)->isDefaultError == true)
						error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
                    ClientInfoIt++;
					continue;
				}
				if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET")
				{
					(*ClientInfoIt)->getRequest = new GETMethod();
					try
					{
						(*ClientInfoIt)->getRequest->callGET(*ClientInfoIt, this->_serverConfiguration);
						if((*ClientInfoIt)->cgiIterator != (*ClientInfoIt)->_currentLocation.CGI.end())
                        {
                            (*ClientInfoIt)->cgiContentType = "";
							(*ClientInfoIt)->cgiContentLength = "0";
							(*ClientInfoIt)->CGIexecutedFile((*ClientInfoIt), this->_serverConfiguration);
						}
					}
					catch(std::exception &e)
					{
						if((*ClientInfoIt)->isDefaultError == true)
							error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
						(*ClientInfoIt)->isErrorOccured = true;
						ClientInfoIt++;
						continue;
					}
				}
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "DELETE")
                {
					try
					{
						(*ClientInfoIt)->checkPathValidation(*ClientInfoIt, this->_serverConfiguration, (*ClientInfoIt)->parsedRequest.requestDataMap["path"]);
						(*ClientInfoIt)->DeleteRequest = new DeleteMethod((*ClientInfoIt)->servedFileName, *ClientInfoIt, this->_serverConfiguration);
						(*ClientInfoIt)->DeleteRequest->deleteTargetedResource();
						(*ClientInfoIt)->DeleteRequest->sendResponse();
					}
					catch (std::exception &e)
					{
						if((*ClientInfoIt)->isDefaultError == true)
							error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
						(*ClientInfoIt)->isErrorOccured = true;
					}
					ClientInfoIt++;
					continue;
                }
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "POST")
				{
					try
					{
						(*ClientInfoIt)->tempPathForLocation = (*ClientInfoIt)->parsedRequest.requestDataMap["path"];
						(*ClientInfoIt)->returnPathWithoutFile((*ClientInfoIt)->tempPathForLocation);
						(*ClientInfoIt)->checkPathValidation(*ClientInfoIt, this->_serverConfiguration, (*ClientInfoIt)->tempPathForLocation);
						(*ClientInfoIt)->postErrorsHandling(this->_serverConfiguration);
						if ((*ClientInfoIt)->isRedirect)
						{
							(*ClientInfoIt)->headerToBeSent += "HTTP/1.1 301 Moved Permanently\r\n"
								+ std::string("Location: ")
								+ (*ClientInfoIt)->_currentLocation.Redirection
								+ "\r\n";
							(*ClientInfoIt)->isSendingHeader = true;
						}
                    }
					catch (const std::exception& e)
					{
                        std::cout << e.what() << std::endl;
						if((*ClientInfoIt)->isDefaultError == true)
                            error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
						(*ClientInfoIt)->isErrorOccured = true;
						ClientInfoIt++;
						continue;
					}
					if ((*ClientInfoIt)->parsedRequest.requestDataMap["Transfer-Encoding:"] == "chunked")
					{
						(*ClientInfoIt)->isChunk = true;
						try
						{
							(*ClientInfoIt)->chunkedRequest = new ChunkedPostRequest;
							(*ClientInfoIt)->chunkedRequest->handleFirstRecv(((*ClientInfoIt)->parsedRequest.requestDataMap["Content-Type:"]).c_str()
															, (*ClientInfoIt)->parsedRequest, (*ClientInfoIt)->isChunkUploadDone);
							if((*ClientInfoIt)->_currentLocation.UploadDirectoryPath == "")
							{
								(*ClientInfoIt)->actionPath = (*ClientInfoIt)->servedFileName;
								(*ClientInfoIt)->servedFileName += (*ClientInfoIt)->parsedRequest.uploadFileName;
								(*ClientInfoIt)->postLocationAbsence(this->_serverConfiguration);
							}
							if ((*ClientInfoIt)->isChunkUploadDone == true)
								(*ClientInfoIt)->preparingMovingTempFile(*ClientInfoIt);
						}
						catch(const std::exception& e)
						{
                            std::cout << e.what() << std::endl;
							error_500((*ClientInfoIt), this->_serverConfiguration.errorInfo["500"]);
							(*ClientInfoIt)->isErrorOccured = true;
							ClientInfoIt++;
							continue;
						}
					}
					else
					{
					 	try
					 	{
							(*ClientInfoIt)->postRequest = new PostMethod(this->_serverConfiguration);
                            (*ClientInfoIt)->postRequest->handleFirstRead(*ClientInfoIt);
                            if ((*ClientInfoIt)->parsedRequest.received == (*ClientInfoIt)->parsedRequest.contentLength)
                                (*ClientInfoIt)->preparingMovingTempFile(*ClientInfoIt);
					 	}
					 	catch (std::exception &e)
					 	{
                             std::cout << e.what() << std::endl;
							if((*ClientInfoIt)->isDefaultError == true)
							{
								(*ClientInfoIt)->isErrorOccured = true;
								error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
							}
							ClientInfoIt++;
							continue;
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
						std::cout << e.what() << std::endl;
						(*ClientInfoIt)->isErrorOccured = true;
						if((*ClientInfoIt)->recvError == true)
						{
							this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
							continue;
						}
						error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
						ClientInfoIt++;
						continue;
                    }
                }
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["Transfer-Encoding:"] == "chunked"
				&& ((*ClientInfoIt)->inReadCgiOut == false))
				{
					try
					{
						(*ClientInfoIt)->chunkedRequest->handleRecv((*ClientInfoIt)->socket, (*ClientInfoIt)->isChunkUploadDone, (*ClientInfoIt)->recvError );
						if ((*ClientInfoIt)->isChunkUploadDone == true)
							(*ClientInfoIt)->preparingMovingTempFile(*ClientInfoIt);
					}
					catch(const std::exception& e)
					{
						(*ClientInfoIt)->isErrorOccured = true;
						std::cerr << e.what() << std::endl;
						if((*ClientInfoIt)->recvError == true)
						{
							this->dropClient((*ClientInfoIt)->socket, ClientInfoIt);
							continue;
						}
                        error_500((*ClientInfoIt), this->_serverConfiguration.errorInfo["500"]);
						ClientInfoIt++;
						continue;
					}
				}
				else if ((*ClientInfoIt)->inReadCgiOut)
				{
					try
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
                                if(n == -1)
                                {
									error_400((*ClientInfoIt), this->_serverConfiguration.errorInfo["400"]);
									(*ClientInfoIt)->isErrorOccured = true;
									(*ClientInfoIt)->inReadCgiOut = false;
									ClientInfoIt++;
									continue;
                                }
								buffer[n] = 0;
								std::string str_buffer(buffer);
								std::string body, mimeType;
								int bef_header = (*ClientInfoIt)->parsedRequest.retIndex(buffer);
								if(bef_header == -1)
								{
									error_400((*ClientInfoIt), this->_serverConfiguration.errorInfo["400"]);
									(*ClientInfoIt)->isErrorOccured = true;
									(*ClientInfoIt)->inReadCgiOut = false;
									ClientInfoIt++;
									continue;
								}
								std::string header_part = str_buffer.substr(0, bef_header);
								(*ClientInfoIt)->parseCgiHeader(header_part);
								std::string contentType = (*ClientInfoIt)->cgiMap["content-type:"];
								std::string contentLength =  (*ClientInfoIt)->cgiMap["content-length:"];
								std::string status = (*ClientInfoIt)->cgiMap["status:"];
								std::string	Location = (*ClientInfoIt)->cgiMap["location:"];
								if(status != "")
									(*ClientInfoIt)->cgiStatus = status;
								if(Location != "")
								{
									(*ClientInfoIt)->headerToBeSent +=  "HTTP/1.1 "
									+ (*ClientInfoIt)->cgiStatus
									+ "\r\n"
									+ std::string("Location: ")
									+ Location
									+ "\r\n\r\n";
									(*ClientInfoIt)->isSendingHeader = true;
									(*ClientInfoIt)->inReadCgiOut = false;
									(*ClientInfoIt)->isRedirect = true;
									ClientInfoIt++;
									continue;
								}
								if(contentLength == "")
								{
									int bytes_available = 0;
									ioctl((*ClientInfoIt)->CgiReadEnd, FIONREAD, &bytes_available);
									(*ClientInfoIt)->cgiBodyLength = bytes_available;
								}
								else
									(*ClientInfoIt)->cgiBodyLength = std::stoi(contentLength);
								mimeType = contentType.substr(0, contentType.find(";"));
								body = str_buffer.substr(bef_header + 4);
								std::string newFile = (*ClientInfoIt)->servedFilesFolder + (*ClientInfoIt)->generateRandString() + get_real_format(mimeType.c_str());
								if((*ClientInfoIt)->cgi_out.is_open()) 
									(*ClientInfoIt)->cgi_out.close();
								(*ClientInfoIt)->cgi_out.open(newFile, std::ios::binary);
								(*ClientInfoIt)->servedFileName = newFile; // case of no upload /testcmd.php or the case of uploaded file example.php
								(*ClientInfoIt)->cgi_out << body;
								(*ClientInfoIt)->isFirstCgiRead = false;
							}
							else
							{
								char buffer[1001];
								ssize_t n;
								int		to_read = 0;
								to_read = ((*ClientInfoIt)->cgiBodyLength > 1024) ? 1024 : (*ClientInfoIt)->cgiBodyLength;
								n = read((*ClientInfoIt)->CgiReadEnd, buffer, to_read);
								if(n == -1)
                                {
									error_400((*ClientInfoIt), this->_serverConfiguration.errorInfo["400"]);
									(*ClientInfoIt)->isErrorOccured = true;
									(*ClientInfoIt)->inReadCgiOut = false;
									ClientInfoIt++;
									continue;
                                }
								(*ClientInfoIt)->cgiBodyLength -= n;
								buffer[n] = 0;
								if(n >= 0)
									(*ClientInfoIt)->cgi_out << buffer;
								if(n < 1000)
								{
									(*ClientInfoIt)->cgi_out.seekp(0, std::ios::end);
									(*ClientInfoIt)->served_size = (*ClientInfoIt)->cgi_out.tellp();
									(*ClientInfoIt)->cgi_out.seekp(0, std::ios::beg);

                                    (*ClientInfoIt)->headerToBeSent = "HTTP/1.1 "
										+ (*ClientInfoIt)->cgiStatus
										+ "\r\n"
										+ std::string("Connection: close\r\n")
										+ std::string("Content-Length: ")
										+ std::to_string((*ClientInfoIt)->served_size)
										+ "\r\n"
										+ std::string("Content-Type: ")
										+ get_mime_format((*ClientInfoIt)->servedFileName.c_str())
										+ "\r\n\r\n";
                                    (*ClientInfoIt)->isSendingHeader = true;
                                    close((*ClientInfoIt)->CgiReadEnd);
                                    (*ClientInfoIt)->inReadCgiOut = false;
                                    (*ClientInfoIt)->PostFinishedCgi = true;
                                    if((*ClientInfoIt)->served.is_open())
                                        (*ClientInfoIt)->served.close();
                                    (*ClientInfoIt)->served.open((*ClientInfoIt)->servedFileName, std::ios::binary);
                                }
							}
						}
					}
					catch (std::exception &e)
					{
						error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
						(*ClientInfoIt)->isErrorOccured = true;
						ClientInfoIt++;
						continue;
					}
				}
			}
		}
		else if(FD_ISSET((*ClientInfoIt)->socket, &(this->_writeFds))) 
		{
				if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "POST")
				{
					if (((*ClientInfoIt)->parsedRequest.received == (*ClientInfoIt)->parsedRequest.contentLength
					|| (*ClientInfoIt)->isChunkUploadDone)
						and (*ClientInfoIt)->isErrorOccured == false and (*ClientInfoIt)->isServing == false
						and (*ClientInfoIt)->inReadCgiOut == false and (*ClientInfoIt)->PostFinishedCgi == false)
					{
						try 
						{
							(*ClientInfoIt)->writeToUploadedFile(*ClientInfoIt);
							if ((*ClientInfoIt)->totalTempFileSize == 0)
							{
								(*ClientInfoIt)->sourceFile.close();
    							(*ClientInfoIt)->destinationFile.close();
								size_t foundPhp = 0;
								size_t foundPerl = 0;
								if((*ClientInfoIt)->isNotUpload)
								{
									foundPhp = (*ClientInfoIt)->actionPath.find(".php");
									foundPerl = (*ClientInfoIt)->actionPath.find(".pl");
								}
								else
								{
									foundPhp = (*ClientInfoIt)->parsedRequest.uploadFileName.find(".php");
									foundPerl = (*ClientInfoIt)->parsedRequest.uploadFileName.find(".pl");
								}
								if((*ClientInfoIt)->isNotUpload)
								{
									chmod((*ClientInfoIt)->actionPath.c_str(),  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
									int acc = access((*ClientInfoIt)->actionPath.c_str(), X_OK | F_OK);
									if(foundPhp == std::string::npos && foundPerl == std::string::npos)
									{
										(*ClientInfoIt)->isDefaultError = false;
										error_400((*ClientInfoIt), this->_serverConfiguration.errorInfo["400"]);
										throw std::runtime_error("executed path was not script");
									}
                                    if(acc == -1)
									{
										(*ClientInfoIt)->isDefaultError = false;
                                        error_404(*ClientInfoIt, this->_serverConfiguration.errorInfo["404"]);
                                        throw std::runtime_error("executed path was not found");
                                    }
									std::ifstream actionPathFile((*ClientInfoIt)->actionPath);
									if(!actionPathFile.is_open())
									{
										(*ClientInfoIt)->isDefaultError = false;
                                        error_404(*ClientInfoIt, this->_serverConfiguration.errorInfo["404"]);
                                        throw std::runtime_error("executed path was not found");
									}
                                    actionPathFile.seekg(0, std::ios::end);
    								int file_size = actionPathFile.tellg();
    								actionPathFile.seekg(0, std::ios::beg);
									actionPathFile.close();
									(*ClientInfoIt)->cgiContentType = foundPhp != std::string::npos ? "text/php" : "text/x-perl-script";
									(*ClientInfoIt)->cgiContentType = "";
									(*ClientInfoIt)->cgiContentLength = std::to_string(file_size);
									(*ClientInfoIt)->CGIexecutedFile((*ClientInfoIt), this->_serverConfiguration);
								}
								else if ((foundPhp != std::string::npos || foundPerl != std::string::npos )
								&& (*ClientInfoIt)->cgiIterator != (*ClientInfoIt)->_currentLocation.CGI.end())
								{
									chmod((*ClientInfoIt)->postFilePath.c_str(),  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
									int acc = access((*ClientInfoIt)->postFilePath.c_str(), X_OK | F_OK);
                                    if(acc == -1)
									{
										(*ClientInfoIt)->isDefaultError = false;
                                        error_404(*ClientInfoIt, this->_serverConfiguration.errorInfo["404"]);
                                        throw std::runtime_error("executed path was not found");
                                    }
									(*ClientInfoIt)->servedFileName = (*ClientInfoIt)->postFilePath;
                                    (*ClientInfoIt)->cgiContentType = foundPhp != std::string::npos ? "text/php" : "text/x-perl-script";
									(*ClientInfoIt)->CGIexecutedFile((*ClientInfoIt), this->_serverConfiguration);
								}
								else
								{

									(*ClientInfoIt)->postRequest->successfulPostRequest(*ClientInfoIt);
									(*ClientInfoIt)->isServing = true;
								}
							}
						}
						catch (std::exception &e)
						{
							std::cout << e.what() << std::endl;
							if((*ClientInfoIt)->isDefaultError == true)
								error_500(*ClientInfoIt, this->_serverConfiguration.errorInfo["500"]);
							(*ClientInfoIt)->isErrorOccured = true;
							ClientInfoIt++;
							continue;
						}
					}
					else if ((((*ClientInfoIt)->isErrorOccured == true) or ((*ClientInfoIt)->isServing == true))
							   or
							   (((*ClientInfoIt)->inReadCgiOut == false) and (*ClientInfoIt)->PostFinishedCgi == true))
					{
						try
						{
							(*ClientInfoIt)->sendResponse();
						}
						catch (std::exception &e)
						{
//							std::cerr << e.what() << std::endl;
							dropClient((*ClientInfoIt)->socket, ClientInfoIt);
							continue;
						}
					}
				} 
				else if ((*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "GET"
				|| (*ClientInfoIt)->parsedRequest.requestDataMap["method"] == "DELETE")
				{
					try
					{
                        (*ClientInfoIt)->sendResponse();
					}
					catch (std::exception &e)
					{
//						std::cerr << e.what() << std::endl;
						dropClient((*ClientInfoIt)->socket, ClientInfoIt);
						continue;
					}
				}
                else
                {
                    try
                    {
                        (*ClientInfoIt)->sendResponse();
                    }
                    catch (std::exception &e)
                    {
//							std::cerr << e.what() << std::endl;
                        dropClient((*ClientInfoIt)->socket, ClientInfoIt);
                        continue;
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

// comments in file.conf

