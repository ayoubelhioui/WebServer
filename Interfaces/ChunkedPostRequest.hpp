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

		// void 	simulatePostReq ( std::ifstream & );
	private:
		std::ofstream	_uploadedFile;
		// char			_buffer[BUFFER_SIZE];
		std::string		_buffer;
		ssize_t			_receivedBytes;
		unsigned int	_currentChunkSize;
		unsigned int	_currentChunkSizeStrLength;
		unsigned int	_fileSize;
		void			_createUploadedFile( const char * );
		void			_receiveChunk ( SOCKET & );
		void			_parseChunk( void );
		void			_writeToUploadedFile( void );
		void			_manipulateCurrentChunkLength ( void );
};

#endif
