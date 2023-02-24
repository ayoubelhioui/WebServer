#ifndef __CHUNKEDPOSTREQUEST_H__
# define __CHUNKEDPOSTREQUEST_H__
# include <fstream>
# include <iostream>
# include <sys/socket.h>
# include "../Header/utils.hpp"
// # include "../errorsHandling/errorsHandling.hpp"
# define BUFFER_SIZE 1024
# define SOCKET int

class ChunkedPostRequest {
	public:
		// ChunkedPostRequest ( ARGS );
		ChunkedPostRequest ( void );
		~ChunkedPostRequest ( void );
		ChunkedPostRequest ( const ChunkedPostRequest & );
		ChunkedPostRequest	&operator= ( const ChunkedPostRequest & );
		void	handleChunk( SOCKET & );
		void	handleFirstChunk ( SOCKET & , const char * );

		// void 	simulatePostReq ( std::ifstream & );
	private:
		std::ofstream	_uploadedFile;
		char			_buffer[BUFFER_SIZE];
		unsigned int	_currentChunkLength;
		ssize_t			_receivedBytes;
		void			_createUploadedFile( const char * );
		void			_receiveChunk ( SOCKET & );
		void			_parseChunk( void );
		void			_writeToUploadedFile( void );
};

#endif
