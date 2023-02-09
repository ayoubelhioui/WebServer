#include "parsing/parsing.hpp"

bool ifLocationSupportUpload(locationBlock &location)
{
    return !(location.UploadDirectoryPath.empty());
}

bool handlingPostRequest(postRequestStruct &postRequest)
{
    if(isNotValidPostRequest(postRequest.requestData))
    {
        error_400(postRequest.clientData, postRequest.clientDataIterator);
        close(postRequest.client->socket);
        std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
        postRequest.clientDataIterator++;
        postRequest.clientData.erase(temp_it);
        return (false);
    }
    if(isTransferEncodingNotChunked(postRequest.requestData))
    {
        error_501(postRequest.clientData, postRequest.clientDataIterator);
        close(postRequest.client->socket);
        std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
        postRequest.clientDataIterator++;
        postRequest.clientData.erase(temp_it);
        return (false);
    }
    std::map<std::string, std::string>::iterator content = postRequest.requestData.find("Content-Length:");
    if(content != postRequest.requestData.end())
    {
        int body_size = std::stoi(postRequest.requestData["Content-Length:"]);
        if(isBodySizeBigger(postRequest.configFileData, body_size, postRequest.client))
        {
            error_413(postRequest.clientData, postRequest.clientDataIterator);
            close(postRequest.client->socket);
            std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
            postRequest.clientDataIterator++;
            postRequest.clientData.erase(temp_it);
            return (false);
        }
    }
    srand(time(NULL));
    int nameGenerating = rand();
    std::stringstream ss;
    ss << nameGenerating;
    std::ofstream file;
//    std::cout << "the content type is : " << postRequest.requestData["Content-Type:"] << std::endl;
//    exit (1);
//    file.open("upload/" + ss.str() + get_mime_format(postRequest.requestData["Content-Type:"].c_str()));
    file.open("upload/" + ss.str() + get_real_format(postRequest.requestData["Content-Type:"].c_str()));
    if (!file.is_open())
        errorPrinting("Couldn't Open Upload File");
    while (file << postRequest.client->request){};
//    std::cout << "im here" << std::endl;
    return (true);
//    char *buffer = new char[1024]();
//    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
//    send(postRequest.client->socket, buffer, strlen(buffer), 0);
//    sprintf(buffer, "Connection: close\r\n");
//    send(postRequest.client->socket, buffer, strlen(buffer), 0);
//    sprintf(buffer, "\r\nREQUEST IS OK");
//    send(postRequest.client->socket, buffer, strlen(buffer), 0);
//    close(postRequest.client->socket);
//    std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
//    postRequest.clientDataIterator++;
//    postRequest.clientData.erase(temp_it);
//    delete [] buffer;
//    return (true);
}