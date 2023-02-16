#ifndef GET_METHOD
#define GET_METHOD
	#include "parsing.hpp"
	class GetMethod{
		public:
			std::string req;
			client_in
			void		callGet(client_info *);
			std::string	handleGetMethod(std::map<std::string, std::string> &, Parsing &);
			GetMethod() : {

			}
			~GetMethod();
	};
#endif