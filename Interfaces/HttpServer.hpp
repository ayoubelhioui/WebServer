#ifndef __HTTPSERVER_H__
# define __HTTPSERVER_H__
# include <sys/socket.h>
# include <sys/select.h>
# include <netdb.h>
# include <string.h>
# include <iostream>
# include <algorithm>
# include <list>
# include "Client.hpp"
# include "ConfigFileParser.hpp"
# include "ServerConfiguration.hpp"
# include "GETMethod.hpp"
# include "../errorsHandling/errorsHandling.hpp"
# define SOCKET int
# define MAXQUEUESIZE 10

class HttpServer {
	public:
		HttpServer ( ServerConfiguration & );
		HttpServer ( void ) ;
		~HttpServer ( void );
		HttpServer ( const HttpServer & );
		HttpServer	&operator= ( const HttpServer & );
		void	setUpHttpServer( void );
		void	setUpMultiplexing ( void );
		void	setClientInfoList ( std::list<ClientInfo> & );
		void	dropClient ( SOCKET &, std::list<ClientInfo *>::iterator & );
		ServerConfiguration	_serverConfiguration;
		std::string			queryString;
		std::string			newPath;
	private:
		SOCKET						_listeningSocket;
		struct addrinfo 			_serverHints;
		std::list<ClientInfo *>		_clientsList;
		fd_set						_readFds;
		fd_set						_writeFds; // * May ruin code
		int 						_maxSocket;
		void	_setUpListeningSocket ( void );
		void	_addClient ( SOCKET );
		void	_selectClients ( void );
		void	_acceptNewConnection ( void );
		void	_serveClients ( void );

};
/*
	A server listen for new connections at a publish address
*/
#endif


