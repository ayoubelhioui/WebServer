#ifndef __MultiHttpServer_H__
# define __MultiHttpServer_H__
# include <list>
# include "HttpServer.hpp"
# include "configFileParse.hpp"

class MultiHttpServer {
	public:
		// MultiHttpServer ( ConfigFileParser & confi  );
		MultiHttpServer ( void );
		~MultiHttpServer ( void );
		// MultiHttpServer ( const MultiHttpServer & );
		// MultiHttpServer	&operator= ( const MultiHttpServer & );
		void	setUpServers( void );
		void	startServers( void );
	private:
		std::list <HttpServer &>	_listOfServers;
		ConfigFileParser			_configuration;
};

#endif
