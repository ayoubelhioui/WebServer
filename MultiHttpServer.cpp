# include "Interfaces/MultiHttpServer.hpp"


/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

// MultiHttpServer::MultiHttpServer ( ARGS )
// {
	
// }

MultiHttpServer::MultiHttpServer ( void )
{

}

MultiHttpServer::~MultiHttpServer ( void )
{
	
}

// MultiHttpServer	&MultiHttpServer::operator= ( const MultiHttpServer &obj )
// {
// 	if (this == &obj)
// 		return (*this);
// 	return (*this);
// }

// MultiHttpServer::MultiHttpServer ( const MultiHttpServer &obj )
// {
// 	*this = obj;
// }


void	MultiHttpServer::setUpServers ( void )
{
	std::list<HttpServer &>::iterator		httpServerIt;

	httpServerIt = this->_listOfServers.begin();

	while (httpServerIt != this->_listOfServers.end())
	{
		httpServerIt->setUpHttpServer();
		httpServerIt++;
	}
	std::cout << "--Servers SetUP Successfully--" << std::endl;
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