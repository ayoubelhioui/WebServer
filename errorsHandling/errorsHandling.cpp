# include "../errorsHandling/errorsHandling.hpp"

bool    isBodySizeBigger(ServerConfiguration &serverConfig, unsigned int bodySize) // STATUS CODE : 403 REQUEST ENTITY TOO LARGE
{
    std::cout << "the bodySize : " << bodySize << "and the client limit size is : " << serverConfig.clientBodyLimit << std::endl;
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


void    error_414(ClientInfo *client)
{
    std::string path = "htmlErrorPages/error414.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 414 Request-URI Too Long\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Connection: close\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    served.read(buffer, file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    delete [] buffer;
}

void    error_501(ClientInfo *client)
{
    std::string path = "htmlErrorPages/error501.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 501 Not Implemented\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Connection: close\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    served.read(buffer, file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    delete [] buffer;
}

void    error_500(ClientInfo *client)
{
    std::string path = "htmlErrorPages/error500.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 500 Internal Server Error\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting(" OK");
    sprintf(buffer, "Connection: close\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK 1");
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK 2");
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK 3 ");
    sprintf(buffer, "\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK 4 ");
    served.read(buffer, file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK 5 ");
    delete [] buffer;
}

void    error_400(ClientInfo *client)
{
    std::string path = "htmlErrorPages/error400.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 400 Bad Request\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Connection: close\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    served.read(buffer, file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    delete [] buffer;
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
void    error_413(ClientInfo *client)
{
    std::string path = "htmlErrorPages/error413.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 413 Request Entity Too Large\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Connection: close\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    served.read(buffer, file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    delete [] buffer;
}

void    error_404(ClientInfo *client)
{
    std::string path = "htmlErrorPages/error404.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 404 Not Found\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Connection: close\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    sprintf(buffer, "\r\n");
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    served.read(buffer, file_size);
    if (send(client->socket, buffer, strlen(buffer), 0) == -1) errorPrinting("SEND IS NOT OK");
    delete [] buffer;
}