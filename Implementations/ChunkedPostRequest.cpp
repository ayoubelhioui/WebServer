# include "../Interfaces/ChunkedPostRequest.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

ChunkedPostRequest::ChunkedPostRequest ( void )
	: 
		_receivedBytes(0),
		_currentChunkSize(0),
		_hexLength(0),
		_writtenBytes(0),
		_chunkContent(nullptr),
		_numberOfRecChunk(0),
		_entered(0)
{ _fileSize = 0;}

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
	this->fileName = generateRandString() + std::string(get_real_format(mimeType));
	// fileName = "~/Desktop/" + generateRandString() + std::string(get_real_format(mimeType));
	this->_uploadedFile.open("/tmp/." + fileName, std::ios::binary);
	if (!this->_uploadedFile.is_open())
	{
		throw std::runtime_error("cannot open chunked folder");
	}

}

void	ChunkedPostRequest::_retrieveChunkSize( char *buffer )
{
	std::string	hexString;
	int		crlfPosition;
	char	*str;

	str = strstr(buffer, "\r\n");
	if (!str)
		throw std::runtime_error("Error in chunked");
	crlfPosition = str - buffer;
	hexString = std::string(buffer, buffer + crlfPosition);
	this->_currentChunkSize = std::stoi(hexString, 0, 16);
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
	{
		delete [] buffer;
		throw std::runtime_error("recv has failed or blocked");
	}
	i = 0;
	while (i < this->_receivedBytes)
	{
		this->_uploadedFile << buffer[i];
		this->_writtenBytes++;
		i++;
	}
	delete [] buffer;
}

void	ChunkedPostRequest::_receiveNextChunkBeginning ( SOCKET &clientSocket, bool &uploadDone )
{
	// char		*buffer;
	unsigned int i;

	this->_writtenBytes = 0;
	char buffer[BUFFER_SIZE];
	this->_receivedBytes = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	if (this->_receivedBytes == -1)
		throw std::runtime_error("recv has failed or blocked");
	this->_retrieveChunkSize(buffer + CRLF);
	if (this->_currentChunkSize == 0)
	{
		this->_uploadedFile.close();
		uploadDone = true;
		std::cout << "DONE WRITTING TO THE FILE" << std::endl;
	}
	i = 0;
	while (i < this->_receivedBytes - this->_hexLength - DOUBLE_CRLF)
	{
		this->_uploadedFile << this->_chunkContent[i];
		this->_writtenBytes++;
		i++;
	}
}
/*------------------------------------------------------------------------*/

void	ChunkedPostRequest::handleFirstRecv ( const char *contentType, ParsingRequest &request, bool &uploadDone )
{
	unsigned int	bodyStart;
	unsigned int	offSet;
	unsigned int	i;

	this->_createUploadedFile(contentType);
	bodyStart = request.retIndex(request.requestHeader) + DOUBLE_CRLF;
	this->_retrieveChunkSize(&request.requestHeader[bodyStart]);
	if (this->_currentChunkSize == 0)
	{
		this->_uploadedFile.close();
		uploadDone = true;
		return ;
	}
	offSet = bodyStart + this->_hexLength + CRLF;
	// if (this->_currentChunkSize == 0)
	// 	{std::cerr << "EMPTY FILE" << std::endl; exit(0);}
	i = 0;
	while (i < MAX_REQUEST_SIZE - offSet)
	{
		this->_uploadedFile << this->_chunkContent[i];
		i++;
	}
	if (this->_currentChunkSize <= MAX_REQUEST_SIZE - offSet)
	{
		this->_uploadedFile.close();
		uploadDone = true;
		return ;
	}
	this->_writtenBytes = i;
}	

void	ChunkedPostRequest::handleRecv( SOCKET &clientSocket, bool &uploadDone)
{
	if (this->_writtenBytes == this->_currentChunkSize)
	{
		this->_receiveNextChunkBeginning(clientSocket, uploadDone);
	}
	else 
	{
		this->_receiveRestOfChunk(clientSocket);
	}
}
