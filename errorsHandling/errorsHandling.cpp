# include "../errorsHandling/errorsHandling.hpp"

bool    isBodySizeBigger(ServerConfiguration &serverConfig, unsigned int bodySize) // STATUS CODE : 403 REQUEST ENTITY TOO LARGE
{
    return (bodySize > serverConfig.clientBodyLimit);
}

bool    isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData) //STATUS CODE: 501 NOT IMPLEMENTED
{
    std::map<std::string, std::string>::iterator Transfer_Encoding = requestData.find("Transfer-Encoding:");
    std::map<std::string, std::string>::iterator Content_length = requestData.find("Content-Length:");
    return (Content_length == requestData.end() and Transfer_Encoding != requestData.end() and Transfer_Encoding->second != "chunked");
}

bool    isNotValidPostRequest(std::map<std::string, std::string> &requestData) // this function should be called when we find a POST request // STATS CODE: 400 BAD REQUEST
{
     std::map<std::string, std::string>::iterator it = requestData.find("Transfer-Encoding:");
     std::map<std::string, std::string>::iterator it1 = requestData.find("Content-Length:");
//     std::cout << (it == requestData.end() and it1 == requestData.end()) << std::endl;
//     std::cout << "transfer : " << it->second << std::endl;
//     std::cout << "content : " << it1->second << std::endl;
     return (it == requestData.end() and it1 == requestData.end());
}

bool isUriTooLong(std::string &Uri)
{
    return (Uri.length() > 2048);
}


void    error_414(ClientInfo *client, std::string &error_page)
{
    std::string path = error_page;
    if(client->served.is_open()) client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);
    std::string error_header = "";

    client->headerToBeSent += "HTTP/1.1 414 Request-URI Too Long\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size) 
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
}

void    error_501(ClientInfo *client, std::string &error_page)
{
    std::string path = error_page;
    if(client->served.is_open()) client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);

    client->headerToBeSent += "HTTP/1.1 501 Not Implemented\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size) 
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
}

void    error_500(ClientInfo *client, std::string &error_page)
{
    std::string path = error_page;
    if(client->served.is_open())
        client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);

    client->headerToBeSent += "HTTP/1.1 500 Internal Server Error\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size)
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n";
    client->isSendingHeader = true;
}

void    error_400(ClientInfo *client, std::string &error_page)
{
    std::string path = error_page;
    if(client->served.is_open()) client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);

    client->headerToBeSent += "HTTP/1.1 400 Bad Request\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size)
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
}

void    error_405(ClientInfo *client, std::string &error_page)
{
    std::string path = error_page;
    if(client->served.is_open()) client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);

    client->headerToBeSent += "HTTP/1.1 405 Method Not Allowed\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size)
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
}

void errorPrinting(const char *errorMessage){
    std::cout << errorMessage << std::endl;
    exit (EXIT_FAILURE);
}

bool isValidNumber(std::string &data){
    for (size_t i = 0; i < data.length(); i++)
        if (!isnumber(data[i]))
            return (false);
    return (true);
}
void    error_413(ClientInfo *client, std::string &error_page)
{
    
    std::string path = error_page;
    if(client->served.is_open()) client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);

    client->headerToBeSent += "HTTP/1.1 413 Request Entity Too Large\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size) 
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
}

void    error_404(ClientInfo *client, std::string &error_page)
{
    std::string path = error_page;
    if(client->served.is_open()) client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);
//    std::string error_header = "";
//
//    error_header += "HTTP/1.1 404 Not Found\r\n"
    client->headerToBeSent += "HTTP/1.1 404 Not Found\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size)  
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
//    if (send(client->socket, client->headerToBeSent.c_str(), client->headerToBeSent.length(), 0) == -1)
//        throw std::runtime_error("send function has failed or blocked");
}