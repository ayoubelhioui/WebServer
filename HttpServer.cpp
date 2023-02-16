# include "Interfaces/HttpServer.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

HttpServer::HttpServer ( configFileParse &config ) : _configuration(config) { }

HttpServer::HttpServer ( void ) : _configuration(configFileParse()) { }

HttpServer::~HttpServer ( void ) { }

/*
HttpServer	&HttpServer::operator= ( const HttpServer &obj )
{
	if (this == &obj)
		return (*this);
	return (*this);
}

HttpServer::HttpServer ( const HttpServer &obj )
{
	*this = obj;
}
*/

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
	
	getaddrinfo(_configuration.serverHost, _configuration.serverPort, &_serverHints, &bindAddress);
	
	_listeningSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (_listeningSocket < 0)
		exit (EXIT_FAILURE);
	std::cout << "Socket Created Successfully" << std::endl;
	optval = 1;
    setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	if (bind(_listeningSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
		exit (EXIT_FAILURE);
	std::cout << "Binded Successfully" << std::endl;
	freeaddrinfo(bindAddress);
	if (listen(_listeningSocket, MAXQUEUESIZE) < 0)
		exit (EXIT_FAILURE);
	std::cout << "Set Up for LISTENING Successfully" << std::endl;
}

void	HttpServer::setUpHttpServer( void )
{
	this->_setUpListeningSocket();
}