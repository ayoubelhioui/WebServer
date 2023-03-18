
# ifndef _CLIENT_H__
# define _CLIENT_H__
# include <iostream>
# include <fstream>
# include <list>
# include <unistd.h>
# include "../Interfaces/RequestParser.hpp"
# include "../Interfaces/POSTMethod.hpp"
# include "../Interfaces/ChunkedPostRequest.hpp"
# include "../Interfaces/GETMethod.hpp"
# include  "../errorsHandling/errorsHandling.hpp"
# include  "../Interfaces/DeleteMethod.hpp"

# include  <sys/ioctl.h>

# define POST "POST"
# define DELETE "DELETE"
# define GET "GET"
class PostMethod;
class GETMethod;
class DeleteMethod;

class ClientInfo {
    public:
		ClientInfo ( void );
		ClientInfo ( ServerConfiguration & );
		~ClientInfo ( void );
		PostMethod			*postRequest;
		ChunkedPostRequest	*chunkedRequest;
		GETMethod 			*getRequest;
		DeleteMethod        *DeleteRequest;
		ClientInfo ( const ClientInfo & );
		ClientInfo	&operator= ( const ClientInfo & );
        void            parseQueryString( void );
		void            CGIexecutedFile( ClientInfo *, ServerConfiguration & );
		void	        parseCgiHeader(std::string &);
        void	        parsingCgiLine(std::string line);
		std::string     generateRandString ( void );
		void            checkPathValidation(ClientInfo *client, ServerConfiguration &serverConfig, std::string &);
        void            searchForIndexFiles();
		void			searchForCgi(ClientInfo *, std::list<LocationBlockParse>::iterator &, std::string &);
		void			returnPathWithoutFile(std::string &);
		bool    		isThereFileLast(std::string &, bool &, int &);
		bool 			_isLocationSupportsCurrentMethod( ClientInfo *, std::string );
		void	        postErrorsHandling( ServerConfiguration & );
		void            postLocationAbsence( ServerConfiguration & );
		void 			preparingMovingTempFile(ClientInfo *client);
		void 			writeToUploadedFile(ClientInfo *client);
		void			sendResponse( void );
		bool			isValidMethod( void );
        ParsingRequest parsedRequest;
        std::string headerToBeSent;
        bool isSendingHeader;
        bool isFirstRead;
        std::ofstream requestBody;
        socklen_t addressLength;
        struct sockaddr_storage address;
        char address_buffer[128];
        SOCKET socket;
        std::ifstream served;
		std::ofstream cgi_out;
		std::string	   servedFileName;	
		std::string	   cgiInput;
        std::string   postFilePath;
		std::string   cgiEnd;
		std::string	  cgiFileEnd;
		std::string	  servedFilesFolder;
		std::string	  tempPathForLocation;
		const char 	  *cgiType;
		std::map<std::string, std::string> cgiMap;
		std::list<std::pair<std::string, std::string> >::iterator cgiIterator;
		std::string             cgiContentLength;
		LocationBlockParse					   					  _currentLocation;
		std::string     										  cgiContentType;
		std::string     										  actionPath;
		bool													  inReadCgiOut;
		int														  CgiReadEnd;
		int														  served_size;
		bool    												  isErrorOccured;
		int														  isServing;
		bool													  stillWaiting;
		bool													  isFirstCgiRead;
		int														  cgiPid;
		bool            										  PostFinishedCgi;
		bool 		    										  isNotUpload;
		bool													  isRedirect;
		int          											  isCreated;
		off_t													  cgiBodyLength;
		int														  readFromCgi;
		std::string												  cgiStatus;
		bool													  isDefaultError;
		bool													  isChunk;
		std::ifstream sourceFile;
		std::ofstream destinationFile;
		int totalTempFileSize;
		int toWrite;
		ServerConfiguration										  serverConfig;
		bool													  isChunkUploadDone;
		bool													   recvError;
};

#endif