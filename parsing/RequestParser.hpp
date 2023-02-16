#ifndef PARSING_REQUEST
#define PARSING_REQUEST
	#include "parsing.hpp"
	class ParsingRequest{
		public :
			int	receivedBytes;
			int bytesToReceive;
			char *requestHeader;
			int contentLength;
			int bodyIndex;
			std::map<std::string, std::string> request_data;
			void	parse();
			void	parsingRequestFirstLine(std::string);
			void	parsingRequest(std::string);
			void	receiveFirstTime(int socket);
			int		retIndex();
			ParsingRequest();
			~ParsingRequest();
	};
#endif