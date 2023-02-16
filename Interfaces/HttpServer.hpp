#ifndef __HTTPSERVER_H__
# define __HTTPSERVER_H__
# include <sys/socket.h>
# include <netdb.h>
# include <string.h>
# include <iostream>

# define SOCKET int
# define MAXQUEUESIZE 10

class configFileParse {
	public:
		char	*serverPort;
		char	*serverHost;
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
		void	_setUpListeningSocket ( void );
};


/*
	A server listen for new connections at a publish address
*/
#endif