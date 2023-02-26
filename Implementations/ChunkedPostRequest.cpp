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

	fileName = "/tmp/" + generateRandString() + std::string(get_real_format(mimeType));
	this->_uploadedFile.open(fileName, std::ios::binary);
	if (this->_uploadedFile.is_open())
		std::cout << fileName << " is created!!" << std::endl;

}

void	ChunkedPostRequest::_retrieveChunkSize( void )
{
	std::string	chunkSizeStr;

	size_t	end = this->_buffer.find("\r\n", 0);
	chunkSizeStr = this->_buffer.substr(0, end);
	try {
		this->_currentChunkSize = std::stoi(chunkSizeStr, 0, 16);
	}
	catch ( const std::exception& excep) {
		std::cerr << excep.what() << std::endl;
	}
	this->_currentChunkSizeStrLength = chunkSizeStr.size();
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
	delete [] buffer;
}

void	ChunkedPostRequest::_receiveNextChunkBeginning ( SOCKET &clientSocket )
{
	char				buffer[BUFFER_SIZE];
	unsigned int		chunkContentBeginningIndex;	
	// unsigned int		chunkBeginningIndex;
	this->_writtenBytes = 0;

	this->_receivedBytes = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	this->_buffer = std::string(buffer);
	this->_retrieveChunkSize();
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

/*------------------------------------------------------------------------*/

void	ChunkedPostRequest::handleFirstRecv ( const char *contentType, ParsingRequest &request )
{
	unsigned int		chunkBeginningIndex;
	unsigned int		chunkContentBeginningIndex;

	this->_createUploadedFile(contentType);
	chunkBeginningIndex = request.retIndex(request.requestHeader) + 4;
	std::cout << "chunkBeginningAsci : " << (int)request.requestHeader[chunkBeginningIndex] << std::endl;
	this->_buffer = std::string(&request.requestHeader[chunkBeginningIndex]);
	this->_retrieveChunkSize();
	std::cout << "chunk Size : " << this->_currentChunkSize << std::endl;
	std::cout << "chunk Size string : " << this->_currentChunkSizeStrLength << std::endl;
	if (this->_currentChunkSize == 0)
		{std::cerr << "EMPTY FILE" << std::endl; exit(0);}
	chunkContentBeginningIndex = CRLF +  this->_currentChunkSizeStrLength;
	std::cout << "Chunk Content START : " << request.requestHeader[chunkContentBeginningIndex] << std::endl;
	unsigned int i = 0;
	while (i < MAX_REQUEST_SIZE - chunkBeginningIndex - chunkContentBeginningIndex)
	{
		this->_uploadedFile << request.requestHeader[chunkBeginningIndex + chunkContentBeginningIndex + i];
		i++;
	}
	this->_writtenBytes = i;
	std::cout << "WRITTEN BYTES : " << this->_writtenBytes << std::endl;
}



void	ChunkedPostRequest::handleRecv( SOCKET &clientSocket )
{
	if (this->_writtenBytes == this->_currentChunkSize)
	{
		this->_receiveNextChunkBeginning(clientSocket);
		this->_numberOfRecChunk++;
	}
	else {
		_receiveRestOfChunk(clientSocket);
		exit(0);
	}
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
