# include "../Interfaces/ChunkedPostRequest.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

// ChunkedPostRequest::ChunkedPostRequest ( ARGS )
// {
	
// }

ChunkedPostRequest::ChunkedPostRequest ( void )
{
	
}

ChunkedPostRequest::~ChunkedPostRequest ( void )
{
	
}

ChunkedPostRequest	&ChunkedPostRequest::operator= ( const ChunkedPostRequest &obj )
{
	if (this == &obj)
		return (*this);
	return (*this);
}

ChunkedPostRequest::ChunkedPostRequest ( const ChunkedPostRequest &obj )
{
	*this = obj;
}
/* ----------------------------------------------------- */
/* ------------------ MEMBER FUNCTIONS ----------------- */
/* ----------------------------------------------------- */

void	ChunkedPostRequest::_createUploadedFile ( const char *mimeType )
{
	std::string	fileName;

	fileName = generateRandString(7) + std::string(get_real_format(mimeType));
	this->_uploadedFile.open(fileName);
	if (this->_uploadedFile.is_open())
		std::cout << fileName << " is created!!" << std::endl;

}

void	ChunkedPostRequest::_receiveChunk( void )
{
	if (-1 == recv(clientSocket, this->_buffer, BUFFER_SIZE, 0))
		std::cerr << "SOCKET " << clientSocket << " NOT READY TO BE READ!!" std::endl;
}

void	ChunkedPostRequest::_parseChunk( void )
{
	
}

void	ChunkedPostRequest::handleChunk ( SOCKET &clientSocket )
{
	
}












































/*
void	ChunkedPostRequest::simulatePostReq ( std::ifstream &infile )
{

	(void)infile;

	std::cout << "\r\n" << std::endl;

	char	buffer[BUFFER_SIZE + 1];

	infile.read(buffer, BUFFER_SIZE);
	buffer[BUFFER_SIZE] = 0;
	std::cout << buffer << std::endl;
	std::cout << "\n\n\n\n" ;
	infile.read(buffer, BUFFER_SIZE);
	buffer[BUFFER_SIZE] = 0;
	std::cout << buffer << std::endl;
}
*/
