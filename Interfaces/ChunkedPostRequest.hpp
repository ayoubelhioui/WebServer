#ifndef __CHUNKEDPOSTREQUEST_H__
# define __CHUNKEDPOSTREQUEST_H__
# include <fstream>

class ChunkedPostRequest {
	public:
		// ChunkedPostRequest ( ARGS );
		ChunkedPostRequest ( void );
		~ChunkedPostRequest ( void );
		ChunkedPostRequest ( const ChunkedPostRequest & );
		ChunkedPostRequest	&operator= ( const ChunkedPostRequest & );
	private:
		std::ofstream	_uploadedFile;
		unsigned int	_currentChunkLength;
		void			_createUploadedFile( void );
		void			_parseChunk( void );
		void			_writeToUploadedFile( void );
			
};


#endif