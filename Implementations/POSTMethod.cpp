# include "../Interfaces/POSTMethod.hpp"
# include "../errorsHandling/errorsHandling.hpp"



PostMethodExceptions::PostMethodExceptions(const std::string &errorMessage) : std::runtime_error(errorMessage) { };

PostMethod::PostMethod(ServerConfiguration &serverConfiguration)
: _serverConfiguration(serverConfiguration)
{}

void PostMethod::preparingPostRequest(ClientInfo *client) {
    client->requestBody.open(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
    if (!client->requestBody.is_open())
        throw (std::runtime_error(TMP_FOLDER_COULDNT_OPEN));
    client->requestBody.write(client->parsedRequest.requestHeader + client->parsedRequest.bodyIndex, client->parsedRequest.received - client->parsedRequest.newBodyIndex - 4);
}

void PostMethod::isValidPostRequest(ClientInfo *client) {
    if(isNotValidPostRequest(client->parsedRequest.requestDataMap))
    {
        error_400(client);
        throw std::runtime_error(BAD_REQUEST_EXCEPTION);
    }
    if(isTransferEncodingNotChunked(client->parsedRequest.requestDataMap))
    {
        error_501(client);
        throw std::runtime_error(TRANSFER_ENCODING_EXCEPTION);
    }
//    std::map<std::string, std::string>::iterator content = client->parsedRequest.requestDataMap.find("Content-Length:");
//    if(content != client->parsedRequest.requestDataMap.end())
//    {
////        std::cout << "the content length is : " << (*_clientInfoIt  )->parsedRequest.contentLength << std::endl;
//        int bodySize = (client->parsedRequest.bodyIndex) - std::stoi(client->parsedRequest.requestDataMap["Content-Length:"]);
//        if(isBodySizeBigger(this->_serverConfiguration, bodySize))
//        {
//            error_413(this->_clientInfoIt);
//            throw std::runtime_error(BODY_SIZE_EXCEPTION);
//        }
//    }
}


void PostMethod::writeInTempFile(ClientInfo *client) {
    client->requestBody.write(client->parsedRequest.requestHeader, client->parsedRequest.receivedBytes);
}

void PostMethod::receiveFromClient(ClientInfo *client){
    client->parsedRequest.bytesToReceive = (client->parsedRequest.received + MAX_REQUEST_SIZE < client->parsedRequest.contentLength) ? MAX_REQUEST_SIZE : client->parsedRequest.contentLength - client->parsedRequest.received;
    client->parsedRequest.receivedBytes = recv(client->socket, client->parsedRequest.requestHeader, client->parsedRequest.bytesToReceive, 0);
    client->parsedRequest.received += client->parsedRequest.receivedBytes;
    client->parsedRequest.requestHeader[client->parsedRequest.receivedBytes] = 0;
}

void PostMethod::serveClient(ClientInfo *client){
    this->receiveFromClient(client);
    this->writeInTempFile(client);
};

void PostMethod::moveFileToUploads(ClientInfo *client)
{
    std::ifstream sourceFile(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
    std::ofstream destinationFile(UPLOADS_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
    if (!destinationFile.is_open() || !sourceFile.is_open())
        throw (std::runtime_error("couldn't Open" + client->parsedRequest.uploadFileName));
    int totalToWrite = client->parsedRequest.received - client->parsedRequest.boundarySize - client->parsedRequest.newBodyIndex - 8, toWrite = 0;
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


void  PostMethod::successfulPostRequest(ClientInfo *client){
    client->requestBody.close();
    moveFileToUploads(client);
    std::string path = "uploadSuccess.html";
    std::ifstream served(path);
    if (!served.is_open())
        throw (PostMethodExceptions(UPLOAD_SUCCESS_FILE_PROBLEM));
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
    delete client->postRequest;
}