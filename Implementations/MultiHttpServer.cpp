# include "../Interfaces/MultiHttpServer.hpp"


/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

MultiHttpServer::MultiHttpServer ( std::list <ServerConfiguration> listOfServerConfig  )
	: _listOfServerConfig(listOfServerConfig), _vectorOfServers(std::vector<HttpServer>(listOfServerConfig.size()))
{}

MultiHttpServer::MultiHttpServer ( void ) {}

MultiHttpServer::~MultiHttpServer ( void )
{
	
}


void	MultiHttpServer::setUpServers ( void )
{
	std::list<ServerConfiguration>::iterator	serverConfigListIt;
	size_t								i;

	i = 0;
	serverConfigListIt = _listOfServerConfig.begin();
	while (i < _vectorOfServers.size())
	{
		_vectorOfServers[i] = HttpServer(*serverConfigListIt);
		// std::cout << "serverHost: " << _vectorOfServers[i]._serverConfiguration.serverHost << std::endl;
		_vectorOfServers[i].setUpHttpServer();
		serverConfigListIt++;
		i++;
	}

}


void MultiHttpServer::startServers(void)
{
	std::list<ServerConfiguration>::iterator	serverConfigListIt;
	size_t								vecIdx;

	signal(SIGPIPE, SIG_IGN);
	while (1)
	{
		vecIdx = 0;
		while (vecIdx < _vectorOfServers.size())
		{
			try
			{
				this->_vectorOfServers[vecIdx].setUpMultiplexing();
			}
			catch (std::exception &e)
			{

			}
			vecIdx++;
		}
	}
}