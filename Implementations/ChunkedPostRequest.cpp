# include "../Interfaces/ChunkedPostRequest.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

// ChunkedPostRequest::ChunkedPostRequest ( ARGS )
// {
	
// }

ChunkedPostRequest::ChunkedPostRequest ( void )
	: _receivedBytes(0),
	  _currentChunkSize(0),
	  _currentChunkSizeStrLength(0),
	  _fileSize(0),
	  _writtenBytes(0),
	  _chunkContent(nullptr)
{}

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

	fileName = generateRandString() + std::string(get_real_format(mimeType));
	this->_uploadedFile.open(fileName);
	if (this->_uploadedFile.is_open())
		std::cout << fileName << " is created!!" << std::endl;

}

void	ChunkedPostRequest::_manipulateCurrentChunkLength( void )
{
	std::string	chunkSizeStr;

	size_t	start = this->_buffer.find("\r\n", 0);
	this->_buffer.erase(start, 2);
	size_t	end = this->_buffer.find("\r\n", 0);
	chunkSizeStr = this->_buffer.substr(0, end);
	// std::cout << "chunkSizeStr : " << chunkSizeStr << std::endl;
	this->_currentChunkSize = std::stoi(chunkSizeStr, 0, 16);
	this->_currentChunkSizeStrLength = chunkSizeStr.size();
	// std::cout << "ChunkSizeStrLen : " << this->_currentChunkSizeStrLength << std::endl;
	// std::cout << "ChunkLen : " << this->_currentChunkSize << std::endl;
	this->_fileSize += this->_currentChunkSize;
}

void	ChunkedPostRequest::_receiveChunk( SOCKET &clientSocket )
{
	unsigned int	bufferSize;
	unsigned int	i;
	char			*buffer;

	bufferSize = (this->_currentChunkSize - this->_writtenBytes) <  BUFFER_SIZE ? (this->_currentChunkSize - this->_writtenBytes) : BUFFER_SIZE;
	buffer = new char[bufferSize]();

	this->_receivedBytes = recv(clientSocket, buffer, bufferSize, 0);
	if (this->_receivedBytes == -1)
		std::cerr << "SOCKET " << clientSocket << " NOT READY TO BE READ!!" << std::endl;
	i = 0;
	while (i < bufferSize)
	{
		this->_uploadedFile << buffer[i];
		this->_writtenBytes++;
		i++;
	}
	delete [] buffer;
}


void	ChunkedPostRequest::_parseChunk( void )
{
	
}
/*------------------------------------------------------------------------*/

void	ChunkedPostRequest::handleFirstRecv ( const char *contentType, ParsingRequest &request )
{
	unsigned int		chunkBeginningIndex;
	unsigned int		chunkContentBeginningIndex;

	this->_createUploadedFile(contentType);
	chunkBeginningIndex = request.retIndex(request.requestHeader) + 2;
	this->_buffer = std::string(&request.requestHeader[chunkBeginningIndex]);
	this->_manipulateCurrentChunkLength();
	if (this->_currentChunkSize == 0)
		{std::cerr << "EMPTY FILE" << std::endl; exit(0);}
	chunkContentBeginningIndex = DOUBLE_CRLF +  this->_currentChunkSizeStrLength;
	unsigned int i = 0;
	while ( request.requestHeader[chunkBeginningIndex + chunkContentBeginningIndex + i])
	{
		this->_uploadedFile << request.requestHeader[chunkBeginningIndex + chunkContentBeginningIndex + i];
		i++;
	}
	this->_writtenBytes = i - 1;
}

void	ChunkedPostRequest::handleRecv( SOCKET &clientSocket )
{
	/* done reading the current chunk */
	if (this->_writtenBytes == this->_currentChunkSize)
		this->_manipulateCurrentChunkLength();
	this->_receiveChunk ( clientSocket );
	if (this->_currentChunkSize == 0)
		this->_uploadedFile.close(); // to be removed
	std::cout << "Written bytes "<<  this->_writtenBytes << std::endl;
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
