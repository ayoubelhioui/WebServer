# include "Interfaces/HttpServer.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

// HttpServer::HttpServer ( ARGS )
// {
	
// }

HttpServer::HttpServer ( void )
{
	
}

HttpServer::~HttpServer ( void )
{
	
}

HttpServer	&HttpServer::operator= ( const HttpServer &obj )
{
	if (this == &obj)
		return (*this);
	return (*this);
}

HttpServer::HttpServer ( const HttpServer &obj )
{
	*this = obj;
}
