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
	  _chunkContent(nullptr),
	  _numberOfRecChunk(0)
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

	fileName = "~/Desktop/" + generateRandString() + std::string(get_real_format(mimeType));
	this->_uploadedFile.open(fileName, std::ios::binary);
	if (this->_uploadedFile.is_open())
		std::cout << fileName << " is created!!" << std::endl;

}

void	ChunkedPostRequest::_parseChunk( void )
{
	
}

void	ChunkedPostRequest::_retrieveChunkSize( void )
{
	std::string	chunkSizeStr;

	std::cout << "this->_buffer[0] " << this->_buffer[0] << std::endl;
	size_t	end = this->_buffer.find("\r\n", 0);
	chunkSizeStr = this->_buffer.substr(0, end);
	// std::cout << "chunkSizeStr : " << chunkSizeStr << std::endl;
	std::cout << "ChunkSize String : " << chunkSizeStr << std::endl;
	this->_currentChunkSize = std::stoi(chunkSizeStr, 0, 16);
	std::cout << "just : " << this->_currentChunkSize << std::endl;
	this->_currentChunkSizeStrLength = chunkSizeStr.size();
	// std::cout << "ChunkSizeStrLen : " << this->_currentChunkSizeStrLength << std::endl;
	// std::cout << "ChunkLen : " << this->_currentChunkSize << std::endl;
	this->_fileSize += this->_currentChunkSize;
}

void	ChunkedPostRequest::_receiveRestOfChunk( SOCKET &clientSocket )
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
	// if (bufferSize < BUFFER_SIZE)
	// 	std::cout << "oh yeah : " << this->_writtenBytes << std::endl;
	delete [] buffer;
}

/*------------------------------------------------------------------------*/

void	ChunkedPostRequest::handleFirstRecv ( const char *contentType, ParsingRequest &request )
{
	unsigned int		chunkBeginningIndex;
	unsigned int		chunkContentBeginningIndex;

	std::cout << "written are " << this->_writtenBytes << std::endl;
	this->_createUploadedFile(contentType);
	chunkBeginningIndex = request.retIndex(request.requestHeader) + 4;
	this->_buffer = std::string(&request.requestHeader[chunkBeginningIndex]);
	this->_retrieveChunkSize();
	if (this->_currentChunkSize == 0)
		{std::cerr << "EMPTY FILE" << std::endl; exit(0);}
	chunkContentBeginningIndex = CRLF +  this->_currentChunkSizeStrLength;
	std::cout << "chunkBeginningIndex " <<	chunkBeginningIndex << std::endl;
	std::cout << "chunkContentBeginningIndex " << chunkContentBeginningIndex << std::endl;
	unsigned int i = 0;
	while ( i < MAX_REQUEST_SIZE - chunkBeginningIndex - chunkContentBeginningIndex - 2)
	{
		this->_uploadedFile << request.requestHeader[chunkBeginningIndex + chunkContentBeginningIndex + i];
		i++;
	}
	this->_writtenBytes = i;
}

void	ChunkedPostRequest::_receiveNextChunkBeginning ( SOCKET &clientSocket )
{
	char				buffer[BUFFER_SIZE];
	unsigned int		chunkContentBeginningIndex;	
	// unsigned int		chunkBeginningIndex;
	this->_writtenBytes = 0;

	this->_receivedBytes = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	this->_buffer = std::string(buffer);
	std::cout << "this->_buffer[0] " << this->_buffer[0] << std::endl;
	this->_retrieveChunkSize();
	std::cout << "CHUNK SIZE " << this->_currentChunkSize << std::endl;
	std::cout << "FILE SIZE " << this->_fileSize << std::endl;
	unsigned int	i = 0;

	chunkContentBeginningIndex = CRLF +  this->_currentChunkSizeStrLength;
	while (i < BUFFER_SIZE - chunkContentBeginningIndex )
	{
		this->_uploadedFile << this->_buffer[chunkContentBeginningIndex + i];
		i++;
	}
	this->_writtenBytes = i;
}

// void	ChunkedPostRequest::handleRecv( SOCKET &clientSocket )
// {
// 	/* done reading the current chunk */
// 	if (this->_writtenBytes == this->_currentChunkSize)
// 	{

// 		this->_retrieveChunkSize();
// 		// std::cout << "current chunk size after reading : " << this->_currentChunkSize << std::endl;
// 	}
// 	else
// 		this->_receiveChunk ( clientSocket );
// 	if (this->_currentChunkSize == 0)
// 		this->_uploadedFile.close(); // to be removed
// 	std::cout << "Written bytes "<<  this->_writtenBytes << std::endl;
// }


void	ChunkedPostRequest::handleRecv( SOCKET &clientSocket )
{
	if (this->_writtenBytes == this->_currentChunkSize)
	{
		std::cout << "WRRITTTEEN ISSSSSSSSSS " << this->_writtenBytes << std::endl;
		std::cout << "CHUNKKKK SIZZZEWEE " << this->_currentChunkSize << std::endl;
		this->_receiveNextChunkBeginning(clientSocket);
		this->_numberOfRecChunk++;
		std::cout << "this->_numberOfRecChunk : " << this->_numberOfRecChunk << std::endl;
		// if (this->_numberOfRecChunk == 2)
			// exit(0);

	}
	else{
		_receiveRestOfChunk(clientSocket);
	}
}


/*
	if (Done reading the current chunk)
		receiveNextChunkBeginning() {
			recv bytes from socket,
			retrieveChunkSize,
		}
	else
		receiveRestOfChunk()

*/




































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
