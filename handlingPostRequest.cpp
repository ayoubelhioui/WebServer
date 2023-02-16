#include "Interfaces/configFileParse.hpp"
#include "includes/postRequest.hpp"

void searchForBoundary(client_info *client)
{
    std::map<std::string, std::string>::iterator content = client->parsedRequest.request_data.find("Content-Type:");
    std::string boundarySavior = content->second;
    std::string newString(client->parsedRequest.requestHeader);
    int newContentIndex = newString.find("filename=");
    int dotPosition = newString.find(".", newContentIndex);
    int DoubleQuotePosition = newString.find("\"", dotPosition);
//    std::cout << "the newstring is : " << std::endl;
    client->parsedRequest.request_data["Content-Type:"] = get_mime_format(newString.substr(dotPosition, DoubleQuotePosition - dotPosition).c_str());
    std::cout << "*****************" << std::endl;
    client->uploadFileName = newString.substr(newContentIndex + 10, dotPosition - newContentIndex - 10) + get_real_format(client->parsedRequest.request_data["Content-Type:"].c_str());
    int newBodyIndex = newString.find("Content-Disposition:");
    newBodyIndex = ret_index(client->parsedRequest.requestHeader) + 4;
    client->parsedRequest.bodyIndex = newBodyIndex;
    client->boundarySize = boundarySavior.length() - boundarySavior.find("=") + 3;
}

bool ifLocationSupportUpload(locationBlock &location)
{
    return !(location.UploadDirectoryPath.empty());
}

//bool isPostMethodAllowed(postRequestStruct &postRequest)
//{
//    std::list<std::string>::iterator allowedMethods = postRequest.configFileData.
//}

void receiveFromClient(client_info *client, int &received)
{
    client->bytesToReceive = (client->received + MAX_REQUEST_SIZE < client->parsedRequest.contentLength) ? MAX_REQUEST_SIZE : client->parsedRequest.contentLength - client->received;
    received = recv(client->socket, client->parsedRequest.requestHeader, client->bytesToReceive, 0);
    // std::cout << "i have received : " << received << " and the content length : " << client->contentLength << std::endl;
    client->received += received;
    client->parsedRequest.requestHeader[received] = 0;
}

void isValidPostRequest(postRequestStruct &postRequest)
{
    if(isNotValidPostRequest(postRequest.client->parsedRequest.request_data))
    {
        error_400(postRequest.clientData, postRequest.clientDataIterator);
        throw postRequestExceptions("Bad Request Exception");
    }
    if(isTransferEncodingNotChunked(postRequest.client->parsedRequest.request_data))
    {
        error_501(postRequest.clientData, postRequest.clientDataIterator);
        throw postRequestExceptions("Transfer Encoding Exception");
    }
    std::map<std::string, std::string>::iterator content = postRequest.client->parsedRequest.request_data.find("Content-Length:");
    if(content != postRequest.client->parsedRequest.request_data.end())
    {
        int body_size = std::stoi(postRequest.client->parsedRequest.request_data["Content-Length:"]);
        if(isBodySizeBigger(postRequest.configFileData, body_size, postRequest.client))
        {
            error_413(postRequest.clientData, postRequest.clientDataIterator);
            throw postRequestExceptions("Body Size Exception");
        }
    }
}

void moveFileToUploads(client_info *client)
{
    std::ifstream sourceFile("/tmp/." + client->uploadFileName, std::ios::binary);
    std::ofstream destinationFile("uploads/" + client->uploadFileName, std::ios::binary);
    if (!destinationFile.is_open() || !sourceFile.is_open())
    {
        std::cout << "Couldn't Open " << client->uploadFileName << std::endl;
        return ;
    }
    int totalToWrite = client->received - client->boundarySize - client->parsedRequest.bodyIndex - 4, toWrite = 0;
    int i = 0;
    while (totalToWrite > 0)
    {
        toWrite = (totalToWrite > 1024) ? 1024 : totalToWrite;
        char buffer[toWrite + 1];
        sourceFile.read(buffer, toWrite);
        buffer[toWrite] = 0;
        destinationFile.write(buffer, toWrite);
        totalToWrite -= toWrite;
    }
    sourceFile.close();
    destinationFile.close();
}

void  successfulPostRequest(std::list<client_info *>::iterator &clientDataIterator, std::list<client_info *> &clientData, client_info *client)
{
    client->requestBody.close();
    moveFileToUploads(client);
    std::string path = "uploadSuccess.html";
    std::ifstream served(path);
    if (!served.is_open())
    {
        std::cout << "Couldn't Open UploadSuccess File" << std::endl;
        return ;
    }
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 201 Created\r\n");
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send(client->socket, buffer, strlen(buffer), 0);
    delete [] buffer;
    dropClient(client->socket, clientDataIterator, clientData);
}
