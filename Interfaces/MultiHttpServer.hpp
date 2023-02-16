#ifndef __MultiHttpServer_H__
# define __MultiHttpServer_H__
# include <list>
# include "HttpServer.hpp"

class MultiHttpServer {
	public:
		// MultiHttpServer ( ARGS );
		MultiHttpServer ( void );
		~MultiHttpServer ( void );
		MultiHttpServer ( const MultiHttpServer & );
		MultiHttpServer	&operator= ( const MultiHttpServer & );
		void	setUpServers( void );
		std::list <HttpServer>	listOfServers;
};


#endif
