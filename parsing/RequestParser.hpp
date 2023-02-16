#ifndef PARSING_REQUEST
#define PARSING_REQUEST
	#include "parsing.hpp"
	class ParsingRequest{
		public :
			
			void	parse(client_info *);
			void	parsingRequestFirstLine(std::string, client_info *);
			void	parsingRequest(std::string, client_info *);
	};
#endif