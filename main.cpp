#include "parsing/parsing.hpp"
#include "includes/postRequest.hpp"

void dropClient(int &clientSocket, std::list<client_info *>::iterator &clientDataIterator, std::list<client_info *> &clientData)
{
    close(clientSocket);
    std::list<client_info *>::iterator temp_it = clientDataIterator;
    clientDataIterator++;
    clientData.erase(temp_it);
}
client_info *get_client(int socket, std::list<client_info *> &data_list)
{
    std::list<client_info *> copy = data_list;
    std::list<client_info *>::iterator it = copy.begin();
    for(; it != copy.end(); it++)
        if(socket == (*it)->socket) return *it;
    client_info *new_node = new client_info;
    new_node->address_length = sizeof(new_node->address);
    data_list.push_front(new_node);
    return new_node;
}

int ret_index(char *str){
  for(int i = 0; str[i]; i++){
    if(!strncmp(&str[i], "\r\n\r\n", 4))
      return i;
  }
  return -1;
}

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

void    error_414(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client)
{
    std::string path = "error_pages/error414.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 414 Request-URI Too Long\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    close((*client)->socket);
    clients_list.erase(client);

}

void parsingRequestFirstLine(std::string &line, std::map<std::string, std::string> &request_data, client_info *client)
{
    std::stringstream str(line);
    std::string word;
    str >> word;
    request_data["method"] = word;
    str >> word;
    if(isUriTooLong(word)) {

    }
    request_data["path"] = word;
    str >> word;
    request_data["httpVersion"] = word;
}

void parsingRequest(std::string &line, std::map<std::string, std::string> &request_data)
{
    std::stringstream str(line);
    std::string word;
    str >> word;
    std::string save = word;
    std::string last;
    while (str >> word)
    {
        last += " ";
        last += word;
    }
    last.erase(0, 1);
    request_data[save] = last;
}

void requestBodyTooLong(client_info *client)
{
   const char *buffer = "HTTP/1.1 400 Bad Request\r\n"
                 "Connection: close\r\n"
                 "Content-Length: 2\r\n"
                 "\r\nAA";
 send(client->socket, buffer, strlen(buffer), 0);
}

void    error_501(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client)
{
    std::string path = "error_pages/error501.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 501 Not Implemented\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    delete [] buffer;
}

void    error_400(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client)
{
    std::string path = "error_pages/error400.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 400 Bad Request\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    delete [] buffer;
}
void    error_413(std::list<client_info *> &clients_list, std::list<client_info *>::iterator &client)
{
    std::string path = "error_pages/error413.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 413 Request Entity Too Large\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send((*client)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send((*client)->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send((*client)->socket, buffer, strlen(buffer), 0);
    delete [] buffer;
}


void	server_start(std::list<Parsing> &servers) {
	std::list<Parsing>::iterator it = servers.begin();
	int server_socket = create_socket(*it);
    std::list<client_info *> client_data;
    int max_socket = 0;
    int prob = 0;
    while(1){
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(server_socket, &reads);
        max_socket = std::max(max_socket, server_socket);
        std::list<client_info *>::iterator client_data_it1 = client_data.begin();
        for(; client_data_it1 != client_data.end(); client_data_it1++){
            FD_SET((*client_data_it1)->socket, &reads);
            std::max(max_socket, server_socket);
        }
        int ret_select = select(max_socket + 1, &reads, NULL, NULL, NULL);
        if(FD_ISSET(server_socket, &reads)){
            client_info *client = get_client(-1, client_data);
            client->socket = accept(server_socket, (struct sockaddr*) &(client->address), &(client->address_length));
             fcntl(client->socket, F_SETFL, O_NONBLOCK);
            FD_SET(client->socket, &reads);
            max_socket = std::max(max_socket, client->socket);
            if(client->socket < 0) std::cerr << "accept function failed\n";
        }
        std::list<client_info *>::iterator client_data_it = client_data.begin();
        int size = client_data.size();
        while(client_data_it != client_data.end())
        {
            int body_size = 0;
            if(FD_ISSET((*client_data_it)->socket, &reads))
            {
                client_info *client = *client_data_it;
                if(MAX_ARRAY_SIZE == client->received)
                {
                    requestBodyTooLong(client);
                    close((*client_data_it)->socket);
                    std:std::list<client_info *>::iterator temp_it = client_data_it;
                    client_data_it++;
                    client_data.erase(temp_it);
                    continue;
                }
                int read_data = MAX_REQUEST_SIZE + client->received < MAX_ARRAY_SIZE ? MAX_REQUEST_SIZE : MAX_ARRAY_SIZE - client->received;
                read_data = recv(client->socket,
                            client->request + client->received,
                            read_data, 0);
                usleep(100);
                if(read_data == -1) {
                    std::cout << "WAS ERROR\n";
                    close(client->socket);
                    std::list<client_info *>::iterator temp_it = client_data_it;
                    client_data_it++;
                    client_data.erase(temp_it);
                    continue;
                }
                client->received += read_data;
                client->request[client->received] = 0;
                if(read_data < MAX_REQUEST_SIZE)
                {
                    std::cout << "**************************************************" << std::endl;
                    std::cout << client->request << std::endl;
                    std::cout << "**************************************************" << std::endl;
                      std::map<std::string, std::string> request_data;
                      int body_index = ret_index(client->request), index = 0, i = 0;
                      std::string stock_header(client->request), line;
                      stock_header = stock_header.substr(0, body_index);
                      std::size_t found = stock_header.find("\r\n");
                      while(found != std::string::npos)
                      {
                            line = stock_header.substr(0, found);
                            if (i == 0){
                                parsingRequestFirstLine(line, request_data, client);
                                i++;
                            }
                            else
                                parsingRequest(line, request_data);
                            stock_header = stock_header.substr(found + 2);
                            found = stock_header.find("\r\n");
                      }
                      
                      if(isUriTooLong(request_data["path"])){
                        error_414(client_data, client_data_it);
                        close(client->socket);
                        client_data.erase(client_data_it);
                        continue;
                    }
                      stock_header = stock_header.substr(found + 1);
                      found = stock_header.find("\r\n");
                      line = stock_header.substr(0, found);
                      parsingRequest(line, request_data);
                      std::map<std::string, std::string>::iterator method = request_data.find("method");
                      if(method->second == "GET"){
                            std::string path = handle_get_method(request_data, *it);
                            std::ifstream served(path, std::ios::binary);
                            served.seekg(0, std::ios::end);
                            int file_size = served.tellg();
                            served.seekg(0, std::ios::beg);
                            char *buffer = new char[1024];
                            sprintf(buffer, "HTTP/1.1 200 OK\r\n");
                            send(client->socket, buffer, strlen(buffer), 0);

                            sprintf(buffer, "Connection: close\r\n");
                            send(client->socket, buffer, strlen(buffer), 0);

                            sprintf(buffer, "Content-Length: %d\r\n", file_size);
                            send(client->socket, buffer, strlen(buffer), 0);

                            sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
                            send(client->socket, buffer, strlen(buffer), 0);

                            sprintf(buffer, "\r\n");
                            send(client->socket, buffer, strlen(buffer), 0);
                            char *s = new char[1024];
                            while (served) {
                                  served.read(buffer, 1024);
                                  int r = served.gcount();
                                  send(client->socket, buffer, r, 0);
                            }
                            close((*client_data_it)->socket);
                            std::list<client_info *>::iterator temp_it = client_data_it;
                            client_data_it++;
                            client_data.erase(temp_it);
                            continue;
                      }
                      if(method->second == "POST")
                      {
                          int boundarySize = 0;
                          searchForBoundary(request_data, body_index, client->request, boundarySize);
                          postRequestStruct postRequest(client, client_data_it, client_data, request_data, *it, body_index, client->received);
                          handlingPostRequest(postRequest, boundarySize);
                      }
                      char *buffer = new char[1024]();
                      sprintf(buffer, "HTTP/1.1 200 OK\r\n");
                      send(client->socket, buffer, strlen(buffer), 0);
                      sprintf(buffer, "Connection: close\r\n");
                      send(client->socket, buffer, strlen(buffer), 0);
                      sprintf(buffer, "\r\nREQUEST IS OK");
                      send(client->socket, buffer, strlen(buffer), 0);
                      close(client->socket);
                      std::list<client_info *>::iterator temp_it = client_data_it;
                      client_data_it++;
                      client_data.erase(temp_it);
                      delete [] buffer;
                }
            }
            client_data_it++;
        }
    }
}

int main(){
    std::list<Parsing> parse;
    readingData(parse);
}
