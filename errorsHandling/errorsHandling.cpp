# include "../webserver.hpp"
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


void    error_414(std::list<ClientInfo *>::iterator &client)
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
    delete [] buffer;
}

void    error_501(std::list<ClientInfo *>::iterator &client)
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

void    error_400(std::list<ClientInfo *>::iterator &client)
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

void    error_413(std::list<ClientInfo *>::iterator &client)
{
    std::string path = "error_pages/error404.html";
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

void    error_404(std::list<ClientInfo *>::iterator &client)
{
    std::string path = "error_pages/error404.html";
    std::ifstream served(path);
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 404 Not Found\n");
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