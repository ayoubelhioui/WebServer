#ifndef __HTTPSERVER_H__
# define __HTTPSERVER_H__

class HttpServer {
	public:
		// HttpServer ( ARGS );
		HttpServer ( void );
		~HttpServer ( void );
		HttpServer ( const HttpServer & );
		HttpServer	&operator= ( const HttpServer & );
};

#endif