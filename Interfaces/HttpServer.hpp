#ifndef __HTTPSERVER_H__
# define __HTTPSERVER_H__
# include <sys/socket.h>
# include <sys/select.h>
# include <netdb.h>
# include <string.h>
# include <iostream>
# include <list>

# define SOCKET int
# define MAXQUEUESIZE 10

class configFileParse {
	public:
		char	*serverPort;
		char	*serverHost;
};
class Client {
	public:
		int		id;
		char	*clientIP;
};
class HttpServer {
	public:
		HttpServer ( configFileParse & );
		HttpServer ( void );
		~HttpServer ( void );
		// HttpServer ( const HttpServer & );
		// HttpServer	&operator= ( const HttpServer & );
		void	setUpHttpServer( void );
	private:
		SOCKET					_listeningSocket;
		struct addrinfo 		_serverHints;
		const configFileParse	_configuration;
		std::list<Client &>		_clientsList;
		fd_set					_readFds;
		fd_set					_writeFds;
		void	_setUpListeningSocket ( void );
		void	_setUpMultiplexing ( void );
};
/*
	A server listen for new connections at a publish address
*/
#endif