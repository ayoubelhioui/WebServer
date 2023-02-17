
#ifndef Client
#define Client

#include "../Interfaces/RequestParser.hpp"
#include "../parsing/parsing.hpp"
class client_info {
	private:
		static int _maxSocket;
    public:
        ParsingRequest parsedRequest;
        std::string uploadFileName;
        int boundarySize;
        int bytesToReceive;
        int received;
        bool isFirstRead;
        bool bodyFirstRead;
        std::ofstream requestBody;
        socklen_t address_length;
        struct sockaddr_storage address;
        char address_buffer[128];
        SOCKET socket;
        std::ifstream served;
        int served_size;
		static void	clients_Setup(int , std::list<client_info *>, fd_set &reads, fd_set &writes);
		static void	checkingClientListenning(int, std::list<client_info *> &, fd_set &, fd_set &);
		static client_info *get_client(int socket, std::list<client_info *> &data_list);
		static void dropClient(int &, std::list<client_info *>::iterator &, std::list<client_info *> &);
		// void       dropClient(int &clientSocket, std::list<client_info *>::iterator &clientDataIterator, std::list<client_info *> &clientData);
		// void    printingParsingData(std::list<Parsing> &parsingData);
		// bool    isValidNumber(std::string &data);
		// void    errorPrinting(const char *errorMessage);
		// void    readingData(std::list<Parsing> &parsingData);
		// void	location_parse(std::list<std::string>::iterator &it, locationBlock &loc);
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
		client_info();
		~client_info();
};

#endif