#ifndef PARSING_REQUEST
#define PARSING_REQUEST
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include "ServerConfiguration.hpp"
#define MAX_REQUEST_SIZE 2000
# define SOCKET int

class ParsingRequest{
	public :
		ParsingRequest();
		~ParsingRequest();
		int bodyIndex;
		int bytesToReceive;
		int contentLength;
		int	receivedBytes;
		char *requestHeader;
		int received;
		int boundarySize;
		int newBodyIndex;
		std::string uploadFileName;
		std::string		queryString;
		std::map<std::string, std::string> requestDataMap;
		bool	isBoundaryExist;
		void	parse();
		void	parsingRequestFirstLine(std::string);
		void	parsingRequest(std::string);
		void	receiveFirstTime ( SOCKET, bool & );
		void	_parsingMiniHeader( void );
		void 	gettingFileName(std::string &);
		void	gettingNewBodyIndex(std::string &);
		int		retIndex(char *);
		bool	checkHost( ServerConfiguration & );
};
#endif
