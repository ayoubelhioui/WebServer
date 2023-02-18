#ifndef GET_METHOD
#define GET_METHOD
	#include "../Interfaces/ConfigFileParser.hpp"
	class GetMethod{
		public:
			void		callGet(client_info *);
			std::string	handleGetMethod(std::map<std::string, std::string> &, Parsing &);
	};
#endif