
# ifndef _CLIENT_H__
# define _CLIENT_H__
# include <iostream>
# include <fstream>
# include <list>
# include "../Interfaces/RequestParser.hpp"
# include "../Interfaces/POSTMethod.hpp"

class PostMethod;

class ClientInfo {
    public:
		ClientInfo ( void );
		~ClientInfo ( void );
		PostMethod *postRequest;
		ClientInfo ( const ClientInfo & );
		ClientInfo	&operator= ( const ClientInfo & );
        ParsingRequest parsedRequest;
        bool isFirstRead;
        std::ofstream requestBody;
        socklen_t addressLength;
        struct sockaddr_storage address;
        char address_buffer[128];
        SOCKET socket;
        std::ifstream served;
        int served_size;
};

#endif