# include "../Interfaces/ChunkedPostRequest.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

ChunkedPostRequest::ChunkedPostRequest ( void )
	: uploadDone(false),
	  _receivedBytes(0),
	  _currentChunkSize(0),
	  _hexLength(0),
	  _fileSize(0),
	  _writtenBytes(0),
	  _chunkContent(nullptr),
	  _numberOfRecChunk(0),
	  _entered(0)
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
	// fileName = "~/Desktop/" + generateRandString() + std::string(get_real_format(mimeType));
	this->_uploadedFile.open(fileName, std::ios::binary);
	if (this->_uploadedFile.is_open())
		std::cout << fileName << " is created!!" << std::endl;

}

void	ChunkedPostRequest::_retrieveChunkSize( char *buffer )
{
	std::string	hexString;
	int		crlfPosition;
	char	*str;

	str = strstr(buffer, "\r\n");
	printf("str: %p\n", str);
	if (!str) 
		exit(0); // * To be removed
	crlfPosition = str - buffer;
	hexString = std::string(buffer, buffer + crlfPosition);
	try {
		this->_currentChunkSize = std::stoi(hexString, 0, 16);
	}
	catch ( const std::exception& excep) {
		std::cerr << excep.what() << std::endl;
	}
	this->_hexLength = hexString.size();
	this->_fileSize += this->_currentChunkSize;
	this->_chunkContent = &buffer[this->_hexLength + CRLF];
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
	while (i < this->_receivedBytes)
	{
		this->_uploadedFile << buffer[i];
		this->_writtenBytes++;
		i++;
	}
	delete [] buffer;
}


void	ChunkedPostRequest::_receiveNextChunkBeginning ( SOCKET &clientSocket )
{
	unsigned int i;

	this->_writtenBytes = 0;
	char buffer[BUFFER_SIZE];
	this->_receivedBytes = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	this->_retrieveChunkSize(buffer + CRLF);
	if (this->_currentChunkSize == 0)
		this->_finishServing();
	i = 0;
	while (i < this->_receivedBytes - this->_hexLength - DOUBLE_CRLF)
	{
		this->_uploadedFile << this->_chunkContent[i];
		this->_writtenBytes++;
		i++;
	}
}
/*------------------------------------------------------------------------*/
void	ChunkedPostRequest::_finishServing ( void )
{
	this->uploadDone = true;
	this->_uploadedFile.close();
}

void	ChunkedPostRequest::handleFirstRecv ( const char *contentType, ParsingRequest &request )
{
	unsigned int	bodyStart;
	unsigned int	offSet;
	unsigned int	i;

	this->_createUploadedFile(contentType);
	bodyStart = request.retIndex(request.requestHeader) + DOUBLE_CRLF;
	this->_retrieveChunkSize(&request.requestHeader[bodyStart]);
	offSet = bodyStart + this->_hexLength + CRLF;
	if (this->_currentChunkSize == 0)
		this->_finishServing();
	i = 0;
	while (i < MAX_REQUEST_SIZE - offSet)
	{
		this->_uploadedFile << this->_chunkContent[i];
		i++;
	}
	if (this->_currentChunkSize < MAX_REQUEST_SIZE)
		this->_finishServing();
	this->_writtenBytes = i;
}

void	ChunkedPostRequest::handleRecv( SOCKET &clientSocket )
{
	if (this->_writtenBytes == this->_currentChunkSize)
	{
		this->_receiveNextChunkBeginning(clientSocket);
	}
	else 
	{
		this->_receiveRestOfChunk(clientSocket);
	}
}
