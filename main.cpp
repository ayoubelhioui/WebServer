#include "Interfaces/configFileParse.hpp"
#include "includes/postRequest.hpp"
#include "Interfaces/RequestParser.hpp"
#include "Interfaces/getMethod.hpp"
#include "Interfaces/Client.hpp"
// void dropClient(int &clientSocket, std::list<client_info *>::iterator &clientInfoItDataIterator, std::list<client_info *> &clientData)
// {
//     close(clientSocket);
//     std::list<client_info *>::iterator temp_it = clientDataIterator;
//     clientDataIterator++;
//     clientData.erase(temp_it);
// }

int create_socket(Parsing &server){
	struct addrinfo server_hints;
    memset(&server_hints, 0, sizeof(server_hints));
    server_hints.ai_family = AF_INET;
    server_hints.ai_socktype = SOCK_STREAM;
    server_hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(server.serverHost.c_str(), server.serverPort.c_str(), &server_hints, &bind_address);

    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (socket_listen < 0) {
        std::cerr << "socket failed" << std::endl;
        exit(1);
    }
    int optval = 1;
    setsockopt(socket_listen, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        std::cerr << "bind failed" << std::endl;
        exit(1);
    }
    freeaddrinfo(bind_address);

    if (listen(socket_listen, 10) < 0) {
        std::cerr << "listen failed" << std::endl;
    }

    return socket_listen;
}

const char *get_mime_format(const char *type){
        const char *last_dot = strrchr(type, '.');
        if(last_dot == NULL) return NULL;
        if (strcmp(last_dot, ".css") == 0) return "text/css";
        if (strcmp(last_dot, ".csv") == 0) return "text/csv";
        if (strcmp(last_dot, ".gif") == 0) return "image/gif";
        if (strcmp(last_dot, ".htm") == 0) return "text/html";
        if (strcmp(last_dot, ".html") == 0) return "text/html";
        if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0) return  "application/javasript";
        if (strcmp(last_dot, ".json") == 0) return "application/json";
        if (strcmp(last_dot, ".png") == 0) return "image/png";
        if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0) return "text/plain";
        if (strcmp(last_dot, ".mp4") == 0) return "video/mp4";
        if (strcmp(last_dot, ".cpp") == 0) return "text/x-c";
        return "application/octet-stream";
}

const char *get_real_format(const char *mime_type){
        if (strcmp(mime_type, "text/css") == 0) return ".css";
        if (strcmp(mime_type, "text/csv") == 0) return ".csv";
        if (strcmp(mime_type, "image/gif") == 0) return ".gif";
        if (strcmp(mime_type, "text/html") == 0) return ".html";
        if (strcmp(mime_type, "text/html") == 0) return ".html";
        if (strcmp(mime_type, "image/x-icon") == 0) return ".x-icon";
        if (strcmp(mime_type, "image/jpeg") == 0) return ".jpeg";
        if (strcmp(mime_type, "image/jpeg") == 0) return ".jpeg";
        if (strcmp(mime_type, "application/javascript") == 0) return ".javascript";
        if (strcmp(mime_type, "application/json") == 0) return ".json";
        if (strcmp(mime_type, "image/png") == 0) return ".png";
        if (strcmp(mime_type, "application/pdf") == 0) return ".pdf";
        if (strcmp(mime_type, "image/svg+xml") == 0) return ".svg+xml";
        if (strcmp(mime_type, "text/plain") == 0) return ".txt";
        if (strcmp(mime_type, "video/mp4") == 0) return ".mp4";
        if (strcmp(mime_type, "text/x-c") == 0) return ".cpp";
        return "";
}

void    error_414(std::list<ClientInfo &> clientsList , std::list<ClientInfo &>::iterator &clientInfoIt)
{
    std::string path = "error_pages/error414.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 414 Request-URI Too Long\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    close(clientInfoIt->socket);
    clients_list.erase(client);

}

void requestBodyTooLong(client_info *client)
{
   const char *buffer = "HTTP/1.1 400 Bad Request\r\n"
                 "Connection: close\r\n"
                 "Content-Length: 2\r\n"
                 "\r\nAA";
 send(client->socket, buffer, strlen(buffer), 0);
}

void    error_501(std::list<ClientInfo &> clientsList , std::list<ClientInfo &>::iterator &clientInfoIt)
{
    std::string path = "error_pages/error501.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 501 Not Implemented\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    close(clientInfoIt->socket);
    delete [] buffer;
}

void    error_400(std::list<ClientInfo &> clientsList , std::list<ClientInfo &>::iterator &clientInfoIt)
{
    std::string path = "error_pages/error400.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 400 Bad Request\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    close(clientInfoIt->socket);
    delete [] buffer;
}

void    error_413(std::list<ClientInfo &> clientsList , std::list<ClientInfo &>::iterator &clientInfoIt)
{
    std::string path = "error_pages/error413.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 413 Request Entity Too Large\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send(clientInfoIt->socket, buffer, strlen(buffer), 0);
    close(clientInfoIt->socket);
    delete [] buffer;
}

void	server_start(std::list<Parsing> &servers)
{
	std::list<Parsing>::iterator it = servers.begin();
	int server_socket = create_socket(*it);
    std::list<client_info *> client_data;
    int max_socket = 0,received = 0;
    while(1) {
        fd_set reads, writes;
        client_info::clients_Setup(server_socket, client_data, reads, writes);
        client_info::checkingClientListenning(server_socket, client_data, reads, writes);
        std::list<client_info *>::iterator client_data_it = client_data.begin();
        int size = client_data.size();
        while (client_data_it != client_data.end())
        {
            client_info *client = *client_data_it;
            if (FD_ISSET(client->socket, &reads))
            {
                if (!client->isFirstRead)
                {
                    client->parsedRequest.receiveFirstTime(client->socket);
                    client->parsedRequest.parse();
                    if(isUriTooLong(client->parsedRequest.request_data["path"]))
                    {
                        error_414(client_data, client_data_it);
                        close(client->socket);
                        client_data.erase(client_data_it);
                        continue;
                    }
                    client->isFirstRead = true;
                    std::map<std::string, std::string>::iterator headerPartIterator = client->parsedRequest.request_data.begin();
                    if (client->parsedRequest.request_data["method"] == "GET"){
                          GETMethod getRequest;
                          getRequest.callGET(client);
                    }
                    else if (client->parsedRequest.request_data["method"] == "DELETE")
                    {
                        // calling get method function.
                        client_data_it++;
                        continue ;
                    }
					else
					{
                          parsingMiniHeader(client);
                          postRequestStruct postRequest(client, client_data_it, client_data, *it);
                          client->requestBody.open("uploads/" + client->uploadFileName, std::ios::binary);
                          client->requestBody.write(client->requestHeader + client->bodyIndex, receivedBytes - client->bodyIndex);
                          client->requestBody.close();
                          exit (1);
					}
                }
                else
                {
                    receiveFromClient(client, received);
                    if (!client->bodyFirstRead)
                    {
                        try
                        {
                            postRequestStruct postRequest(client, client_data_it, client_data, *it);
                            isValidPostRequest(postRequest);
                            client->bodyFirstRead = true;
                            searchForBoundary(client);
                            client->requestBody.open("/tmp/." + client->uploadFileName, std::ios::binary);
                        }
                        catch (postRequestExceptions &e)
                        {
                            std::cout << "Post Request Problem : Exception caught" << std::endl;
                            dropClient(client->socket, client_data_it, client_data);
                            continue;
                        }
                        if (!client->requestBody.is_open())
                        {
                            std::cout << "Couldn't Open Upload File" << std::endl;
                            dropClient(client->socket, client_data_it, client_data);
                            continue ;
                        }
                        client->requestBody.write(client->parsedRequest.requestHeader + client->parsedRequest.bodyIndex, received - client->parsedRequest.bodyIndex);
                    }
                    else
                        client->requestBody.write(client->parsedRequest.requestHeader, received);
                    if (client->received == client->parsedRequest.contentLength)
                    {
                        successfulPostRequest(client_data_it, client_data, client);
                        continue ;
                    }
                }
            }
            if(FD_ISSET(client->socket, &writes)){
                char *s = new char[1024]();
                client->served.read(s, 1024);
                int r = client->served.gcount();
                send(client->socket, s, r, 0);
                if(r < 1024){
                    close(client->socket);
                    std::list<client_info *>::iterator temp_it = client_data_it;
                    client_data_it++;
                    client_data.erase(temp_it);
                    continue;
                }
            }
            client_data_it++;
        }
    }
}

int main(int ac, char **av, char **env)
{
    if (ac > 2)
        errorPrinting("too many arguments.");



    std::string configFilePath = (ac == 2) ? std::string(av[1]) : DEFAULT_CONFIG_FILE_NAME;
    
	std::list<configFileParse> configParse;
    std::list<std::string> configFileInfo;
    configFileParse::readingDataFromFile(configFileInfo, configFilePath);
    configFileParse::startParsingFile(configFileInfo, configParse);
    configFileParse::printingParsingData(configParse);
}














# include "Interfaces/MultiHttpServer.hpp"
# include "Interfaces/Client.hpp"

int main ( void)
{
	ConfigFileParser		ConfigFile;
	MultiHttpServer			MultiServers;
	std::list<ClientInfo>	ClientInfoList;

	ConfigFile.parseConfigFile();
	MultiServers = MultiHttpServer(ConfigFile);
	MultiServers.setUpServers(ClientInfoList);
	MultiServers.startServers();

	
}
