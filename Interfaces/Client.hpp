
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
	// static void	checkingClientListenning(int, std::list<ClientInfo> &, fd_set &, fd_set &);
	// static void	clients_Setup(int , std::list<ClientInfo>, fd_set &reads, fd_set &writes);
	// static ClientInfo *get_client(int socket, std::list<ClientInfo> &data_list);
		// static void dropClient(int &, std::list<ClientInfo>::iterator &, std::list<ClientInfo> &);
		// void       dropClient(int &clientSocket, std::list<ClientInfo *>::iterator &clientDataIterator, std::list<client_info *> &clientData);
		// void    printingParsingData(std::list<Parsing> &parsingData);
		// bool    isValidNumber(std::string &data);
		// void    errorPrinting(const char *errorMessage);
		// void    readingData(std::list<Parsing> &parsingData);
		// bool    all_empty(std::string str);
		// void	set_upload_file(std::vector<std::string> &vec, locationBlock &loc);
		// void	set_cgi(std::vector<std::string> &vec, locationBlock &loc);
		// void	set_indexes(std::vector<std::string> &vec, locationBlock &loc);
		// void	set_root(std::vector<std::string> &vec, locationBlock &loc);
		// void	set_dirlisting(std::vector<std::string> &vec, locationBlock &loc);
		// void	fill_redirection(std::vector<std::string> &vec, locationBlock &loc);
		// void	fill_allow_methods(std::vector<std::string> &vec, locationBlock &loc);
		// void	get_path(std::vector<std::string> &vec, locationBlock &loc);
		// void	server_start(std::list<Parsing> &servers);
		// bool    isNotValidPostRequest(std::map<std::string, std::string> &requestData);
		// bool    isUriTooLong(std::string &Uri);
		// std::string handle_get_method(std::map<std::string, std::string> &request, Parsing &server);
};

#endif