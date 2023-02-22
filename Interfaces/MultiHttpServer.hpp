#ifndef __MultiHttpServer_H__
# define __MultiHttpServer_H__
# include <vector>
# include <iostream>
# include "HttpServer.hpp"
# include "ConfigFileParser.hpp"

class MultiHttpServer {
	public:
		MultiHttpServer ( std::list <ServerConfiguration>  );
		MultiHttpServer ( void );
		~MultiHttpServer ( void );
		// MultiHttpServer ( const MultiHttpServer & );
		// MultiHttpServer	&operator= ( const MultiHttpServer & );
		void	setUpServers( void );
		void	startServers( void );
	private:

		std::list <ServerConfiguration> _listOfServerConfig;
		std::vector<HttpServer>	_vectorOfServers;
};

#endif
