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

void parsingRequestFirstLine(std::string &line, client_info *client)
{
    std::stringstream str(line);
    std::string word;
    str >> word;
    client->request_data["method"] = word;
    str >> word;
    if(isUriTooLong(word)) {

    }
    client->request_data["path"] = word;
    str >> word;
    client->request_data["httpVersion"] = word;
}

void parsingRequest(std::string &line,  client_info *client)
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
    if (save == "Content-Length:")
        client->contentLength = atoi(last.c_str());
    client->request_data[save] = last;
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

void	server_start(std::list<Parsing> &servers)
{
	std::list<Parsing>::iterator it = servers.begin();
	int server_socket = create_socket(*it);
    std::list<client_info *> client_data;
    int max_socket = 0,received = 0;
    int prob = 0;
    while(1) {
        fd_set reads, writes;
        FD_ZERO(&reads);
        FD_SET(server_socket, &reads);
        FD_ZERO(&writes);
        FD_SET(server_socket, &writes);
        max_socket = std::max(max_socket, server_socket);
        std::list<client_info *>::iterator client_data_it1 = client_data.begin();
        for (; client_data_it1 != client_data.end(); client_data_it1++)
        {
            FD_SET((*client_data_it1)->socket, &reads);
            FD_SET((*client_data_it1)->socket, &writes);
            std::max(max_socket, (*client_data_it1)->socket);
        }
        int ret_select = select(max_socket + 1, &reads, &writes, NULL, NULL);
        if (FD_ISSET(server_socket, &reads))
        {
            client_info *client = get_client(-1, client_data);
            client->socket = accept(server_socket, (struct sockaddr *) &(client->address), &(client->address_length));
//            fcntl(client->socket, F_SETFL, O_NONBLOCK);
            FD_SET(client->socket, &reads);
            FD_SET(client->socket, &writes);
            max_socket = std::max(max_socket, client->socket);
            if (client->socket < 0) std::cerr << "accept function failed\n";
        }
        std::list<client_info *>::iterator client_data_it = client_data.begin();
        int size = client_data.size();
        while (client_data_it != client_data.end())
        {
            client_info *client = *client_data_it;
            if (FD_ISSET(client->socket, &reads))
            {
                if (!client->isFirstRead)
                {
                    int receivedBytes = recv(client->socket, client->requestHeader, MAX_REQUEST_SIZE, 0);
                      client->requestHeader[receivedBytes] = 0;
//                      std::cout << "*********************************" << std::endl;
//                    std::cout << client->requestHeader << std::endl;
//                    std::cout << "*********************************" << std::endl;
                      client->bytesToReceive += receivedBytes;
                      client->bodyIndex = ret_index(client->requestHeader);
                      int index = 0, i = 0;
                      std::string headerPart(client->requestHeader), line, bodyPart(headerPart);
                      headerPart = headerPart.substr(0, client->bodyIndex);
                      std::size_t found = headerPart.find("\r\n");
                      while(found != std::string::npos)
                      {
                        line = headerPart.substr(0, found);
                        if (i == 0)
                        {
                            parsingRequestFirstLine(line, client);
                            i++;
                        }
                        else
                            parsingRequest(line, client);
                        headerPart = headerPart.substr(found + 2);
                        found = headerPart.find("\r\n");
                      }
                      if(isUriTooLong(client->request_data["path"]))
                      {
                        error_414(client_data, client_data_it);
                        close(client->socket);
                        client_data.erase(client_data_it);
                        continue;
                     }
                      headerPart = headerPart.substr(found + 1);
                      found = headerPart.find("\r\n");
                      line = headerPart.substr(0, found);
                      parsingRequest(line, client);
//                      std::map<std::string, std::string>::iterator m = client->request_data.begin();
//                       while (m != client->request_data.end())
//                        {
//                            std::cout << "the data is : " << m->first << " " << m->second << std::endl;
//                            m++;
//                        }
//                       exit (1);
                      client->isFirstRead = true;
                      std::map<std::string, std::string>::iterator headerPartIterator = client->request_data.begin();
                      if (client->request_data["method"] == "GET")
                      {
                            std::string path = handle_get_method(client->request_data, *it);
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
                            while (served)
                            {
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
                      else if (client->request_data["method"] == "DELETE")
                      {
                          // calling get method function.
                          client_data_it++;
                          continue ;
                      }
                }
                else
                {
                    exit (1);
//                    client->bytesToReceive = (client->received + MAX_REQUEST_SIZE < client->contentLength) ? MAX_REQUEST_SIZE : client->contentLength - client->received;
//                    received = recv(client->socket, client->requestHeader, client->bytesToReceive, 0);
//                    client->received += received;
//                    client->requestHeader[received] = 0;
//                        std::map<std::string, std::string>::iterator m = client->request_data.begin();
//                       while (m != client->request_data.end())
//                        {
//                            std::cout << "the data is : " << m->first << " " << m->second << std::endl;
//                            m++;
//                        }
//                       exit (1);
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
//                        client->bodyFirstRead = true;
//                        searchForBoundary(client);
//                        client->requestBody.open("/tmp/." + client->uploadFileName, std::ios::binary);
                        if (!client->requestBody.is_open())
                        {
                            std::cout << "Couldn't Open Upload File" << std::endl;
                            dropClient(client->socket, client_data_it, client_data);
                            continue ;
                        }
                        client->requestBody.write(client->requestHeader + client->bodyIndex, received - client->bodyIndex);
                    }
                    else
                        client->requestBody.write(client->requestHeader, received);
                    if (client->received == client->contentLength)
                    {
                        successfulPostRequest(client_data_it, client_data, client);
                        continue ;
                    }
                }
            }
            client_data_it++;
        }
    }
}

int main(){
//    std::cout << remove("tmp/testingwrite.cpp") << std::endl;
    std::list<Parsing> parse;
    readingData(parse);
}
