#ifndef __CHUNKEDPOSTREQUEST_H__
# define __CHUNKEDPOSTREQUEST_H__
# include <fstream>
# include <iostream>
# include <cstring>
# include <sys/socket.h>
# include "../Header/utils.hpp"
# include "../Interfaces/RequestParser.hpp"
// # include "../errorsHandling/errorsHandling.hpp"
# define BUFFER_SIZE 1024
# define MAX_REQUEST_SIZE 2000
# define CRLF 2
# define DOUBLE_CRLF 4
# define SOCKET int

class ChunkedPostRequest {
	public:
		// ChunkedPostRequest ( ARGS );
		ChunkedPostRequest ( void );
		~ChunkedPostRequest ( void );
		ChunkedPostRequest ( const ChunkedPostRequest & );
		ChunkedPostRequest	&operator= ( const ChunkedPostRequest & );
		void	handleRecv( SOCKET & );
		void	handleFirstRecv ( const char *, ParsingRequest & );
	private:
		std::ofstream	_uploadedFile;
		std::string		_buffer;
		ssize_t			_receivedBytes;
		unsigned int	_currentChunkSize;
		unsigned int	_currentChunkSizeStrLength;
		unsigned int	_fileSize;
		unsigned int	_writtenBytes;
		char			*_chunkContent;
		int				_numberOfRecChunk;
		void			_createUploadedFile( const char * );
		void			_receiveRestOfChunk ( SOCKET & );
		void			_receiveNextChunkBeginning ( SOCKET & );
		void			_parseChunk( void );
		void			_writeToUploadedFile( void );
		void			_retrieveChunkSize ( void );
};

#endif
