#include "parsing/parsing.hpp"

client_info *get_client(int socket, std::list<client_info *> &data_list){
    std::list<client_info *> copy = data_list;
    std::list<client_info *>::iterator it = copy.begin();
    for(; it != copy.end(); it++)
        if(socket == (*it)->socket) return *it;
    client_info *new_node = new client_info;
    new_node->address_length = sizeof(new_node->address);
    data_list.push_front(new_node);
    return new_node;
}

int client->parsedRequest.retIndex(char *str){
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


//bool isValidUri(std::string &Uri)
//{
//
//    std::string allowedCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'@()*+,;=%";
//    for (int i = 0; i < Uri.length(); i++)
//        if (allowedCharacters.find(Uri[i]) == std::string::npos)
//            return (false);
//    return (true);
//}


void parsingRequestFirstLine(std::string &line, client_info *client)
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
    request_data[save] = last;
}

//void requestBodyTooLong(client_info *client)
//{
//    const char *buffer = "HTTP/1.1 400 Bad Request\r\n"
//                  "Connection: close\r\n"
//                  "Content-Length: 2\r\n"
//                  "\r\nAA";
//  send(client->socket, buffer, strlen(buffer), 0);
//}

//bool isBodySizeBigger(Parsing &servers, int bodySize, client_info *client)
//{
//    if (bodySize > servers.clientBodyLimit)
//    {
//        requestBodyTooLong(client);
//        return (true);
//    }
//    return (false);
//}


void	server_start(std::list<Parsing> &servers) {
	std::list<Parsing>::iterator it = servers.begin();
	int server_socket = create_socket(*it);
    std::list<client_info *> client_data;
    int max_socket = 0;
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
            FD_SET(client->socket, &reads);
            max_socket = std::max(max_socket, client->socket);
            if(client->socket < 0) std::cerr << "accept function failed\n";
        }
        std::list<client_info *>::iterator client_data_it = client_data.begin();
        int size = client_data.size();
        while(client_data.size())
        {
            int body_size = 0;
            if(FD_ISSET((*client_data_it)->socket, &reads)){
                client_info *client = *client_data_it;
            if(MAX_ARRAY_SIZE == client->received)
            {
                // requestBodyTooLong(client);
                // std::cout << "i'm in error\n" << std::endl;
                close((*client_data_it)->socket);
                client_data.erase(client_data_it);
                continue;
            }
                    int read = MAX_REQUEST_SIZE + client->received < MAX_ARRAY_SIZE ? MAX_REQUEST_SIZE : MAX_ARRAY_SIZE - client->received;
				    // int read = 10;
                    read = recv(client->socket,
                                client->request + client->received,
                                read, 0);
                    std::cout << "read " << read << std::endl;
                    if(read == -1) {};
                client->received += read;
                client->request[client->received] = 0;
                if(read < MAX_REQUEST_SIZE) {
                      std::map<std::string, std::string> request_data;
                      int body_index = client->parsedRequest.retIndex(client->request), index = 0, i = 0;
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
                        continue;
                    }
                      stock_header = stock_header.substr(found + 1);
                      found = stock_header.find("\r\n");
                      line = stock_header.substr(0, found);
                      parsingRequest(line, request_data);
                      std::map<std::string, std::string>::iterator method = request_data.find("method");
                      if(method->second == "GET"){
                            std::string path = request_data["path"];
                            path.erase(0, 1);
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
                            client_data.erase(client_data_it);
                            continue;
                      }
                      if(method->second == "POST"){

                        if(isNotValidPostRequest(request_data)){
                            error_400(client_data, client_data_it);
                            continue;
                        }
                        if(isTransferEncodingNotChunked(request_data)){
                            error_501(client_data, client_data_it);
                            continue;
                        }
                        std::map<std::string, std::string>::iterator content = request_data.find("Content-Length:");
                        if(content != request_data.end()){
                            body_size = std::stoi(request_data["Content-Length:"]);
                            if(isBodySizeBigger(*it, body_size, client)){
                                error_413(client_data, client_data_it);
                                continue;
                            }
                        }
                      }
                      char *buffer = new char[1024]();
                      sprintf(buffer, "HTTP/1.1 200 OK\r\n");
                    send(client->socket, buffer, strlen(buffer), 0);
                    sprintf(buffer, "Connection: close\r\n");
                    send(client->socket, buffer, strlen(buffer), 0);
                    sprintf(buffer, "\r\nREQUEST IS OK");
                    send(client->socket, buffer, strlen(buffer), 0);
                    close((*client_data_it)->socket);
                    client_data.erase(client_data_it);
                    delete [] buffer;
                      // POST METHOD USAGE :
                      // int body_size = std::stoi(request_data["Content-Length:"]);
                      // std::vector<unsigned char> binary_data(body_size);
                      // std::string format = "test";
                      // format += get_real_format(request_data["Content-Type:"].c_str());
                      // std::ofstream outfile(format, std::ios::binary);
                      // outfile.write(client->request + body_index + 4, body_size);
                      // outfile.close();
                      // std::map<std::string, std::string>::iterator it = request_data.begin();
                      // std::vector<unsigned char> binary_data(file_size);
                      // infile.read(reinterpret_cast<char*>(binary_data.data()), file_size);
                      // infile.close();
                      // std::ofstream outfile("test.mp4", std::ios::binary);
                      // outfile.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size() / 4);
                      // outfile.close();
                }
            }
        }
    }
}

int main(){
    std::list<Parsing> parse;
    readingData(parse);
}
