# include "Interfaces/MultiHttpServer.hpp"


/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

MultiHttpServer::MultiHttpServer ( std::list <ServerConfiguration &> listOfServerConfig  )
	: _listOfServerConfig(listOfServerConfig)
{
	this->_vectorOfServers = std::vector<HttpServer &>(this->_listOfServerConfig.size());
}

MultiHttpServer::MultiHttpServer ( void ) {}

MultiHttpServer::~MultiHttpServer ( void )
{
	
}

// void	MultiHttpServer::setUpServers ( void )
// {
// 	std::vector<HttpServer &>::iterator		httpServerIt;

// 	httpServerIt = this->_vectorOfServers.begin();

// 	while (httpServerIt != this->_vectorOfServers.end())
// 	{
// 		httpServerIt
// 		httpServerIt->setUpHttpServer();
// 		httpServerIt++;
// 	}
// 	std::cout << "--Servers SetUP Successfully--" << std::endl;
// }

void	MultiHttpServer::setUpServers ( void )
{
	std::list<ServerConfiguration &>::iterator	serverConfigListIt;
	int											i;

	i = 0;
	serverConfigListIt = _listOfServerConfig.begin();
	while (i < _vectorOfServers.size())
	{
		_vectorOfServers[i] = HttpServer(*serverConfigListIt);
		_vectorOfServers[i].setUpHttpServer();
		serverConfigListIt++;
		i++;
	}

}


void MultiHttpServer::startServers(void)
{
	std::list<HttpServer &>::iterator	HttpServerIt;


	while (1)
	{
		HttpServerIt = this->_listOfServers.begin();
		while (HttpServerIt != this->_listOfServers.end())
		{
			HttpServerIt->setUpMultiplexing();
			HttpServerIt++;
		}

	}
	
}