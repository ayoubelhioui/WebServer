
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
		std::ofstream cgi_out;
		std::string	   servedFileName;	
		std::string	  currentServerFile;
		bool			inReadCgiOut;
		int				CgiReadEnd;
        int				served_size;
		bool			stillWaiting;
		bool			isFirstCgiRead;
		int				cgiPid;
		// static void	clients_Setup(int , std::list<ClientInfo>, fd_set &reads, fd_set &writes);
		// static void	checkingClientListenning(int, std::list<ClientInfo> &, fd_set &, fd_set &);
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