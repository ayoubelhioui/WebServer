# include "../Interfaces/POSTMethod.hpp"
# include "../errorsHandling/errorsHandling.hpp"
PostMethod::PostMethod(ServerConfiguration &serverConfiguration, std::list<ClientInfo *>::iterator &clientInfoIt, std::list<ClientInfo *> &clientsList)
    : _serverConfiguration(serverConfiguration), _clientInfoIt(clientInfoIt), _clientsList(clientsList)
    {}

PostMethod::PostMethod() {};


PostMethod::~PostMethod() {}

PostMethodExceptions::PostMethodExceptions(const std::string &errorMessage) : std::runtime_error(errorMessage) { };




void PostMethod::preparingPostRequest() {
    (*this->_clientInfoIt)->requestBody.open(TMP_FOLDER_PATH + (*_clientInfoIt)->parsedRequest.uploadFileName, std::ios::binary);
    if (!(*_clientInfoIt)->requestBody.is_open())
        throw (std::runtime_error(TMP_FOLDER_COULDNT_OPEN));
    (*_clientInfoIt)->requestBody.write((*_clientInfoIt)->parsedRequest.requestHeader + (*_clientInfoIt)->parsedRequest.bodyIndex, (*_clientInfoIt)->parsedRequest.receivedBytes - (*_clientInfoIt)->parsedRequest.bodyIndex);
    (*_clientInfoIt)->requestBody.close();
}

void PostMethod::isValidPostRequest() {
    if(isNotValidPostRequest((*_clientInfoIt)->parsedRequest.requestDataMap))
    {
        error_400(this->_clientInfoIt);
        throw std::runtime_error(BAD_REQUEST_EXCEPTION);
    }
    if(isTransferEncodingNotChunked((*_clientInfoIt)->parsedRequest.requestDataMap))
    {
        error_501(this->_clientInfoIt);
        throw std::runtime_error(TRANSFER_ENCODING_EXCEPTION);
    }
//    std::map<std::string, std::string>::iterator content = (*_clientInfoIt)->parsedRequest.requestDataMap.find("Content-Length:");
//    if(content != (*_clientInfoIt)->parsedRequest.requestDataMap.end())
//    {
////        std::cout << "the content length is : " << (*_clientInfoIt  )->parsedRequest.contentLength << std::endl;
//        int bodySize = ((*_clientInfoIt)->parsedRequest.bodyIndex) - std::stoi((*_clientInfoIt)->parsedRequest.requestDataMap["Content-Length:"]);
//        if(isBodySizeBigger(this->_serverConfiguration, bodySize))
//        {
//            error_413(this->_clientInfoIt);
//            throw std::runtime_error(BODY_SIZE_EXCEPTION);
//        }
//    }
}


void PostMethod::writeInTempFile() {
    (*_clientInfoIt)->requestBody.write((*_clientInfoIt)->parsedRequest.requestHeader, (*_clientInfoIt)->bytesToReceive);
}

void PostMethod::receiveFromClient(){
    int received = 0;
    std::cout << "the received : " << std::endl;
    (*_clientInfoIt)->bytesToReceive = ((*_clientInfoIt)->received + MAX_REQUEST_SIZE < (*_clientInfoIt)->parsedRequest.contentLength) ? MAX_REQUEST_SIZE : (*_clientInfoIt)->parsedRequest.contentLength - (*_clientInfoIt)->received;
    received = recv((*_clientInfoIt)->socket, (*_clientInfoIt)->parsedRequest.requestHeader, (*_clientInfoIt)->bytesToReceive, 0);
    (*_clientInfoIt)->received += received;
    (*_clientInfoIt)->parsedRequest.requestHeader[received] = 0;
}

void PostMethod::serveClient(){
    this->receiveFromClient();
    this->writeInTempFile();
};


void PostMethod::moveFileToUploads()
{
    std::ifstream sourceFile(TMP_FOLDER_PATH + (*_clientInfoIt)->parsedRequest.uploadFileName, std::ios::binary);
    std::ofstream destinationFile(UPLOADS_FOLDER_PATH + (*_clientInfoIt)->parsedRequest.uploadFileName, std::ios::binary);
    if (!destinationFile.is_open() || !sourceFile.is_open())
        throw (PostMethodExceptions("couldn't Open" + (*_clientInfoIt)->parsedRequest.uploadFileName));
    int totalToWrite = (*_clientInfoIt)->received - (*_clientInfoIt)->boundarySize - (*_clientInfoIt)->parsedRequest.bodyIndex - 4, toWrite = 0;
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


void  PostMethod::successfulPostRequest(){
    (*_clientInfoIt)->requestBody.close();
    moveFileToUploads();
    std::string path = "uploadSuccess.html";
    std::ifstream served(path);
    if (!served.is_open())
        throw (PostMethodExceptions(UPLOAD_SUCCESS_FILE_PROBLEM));
    std::cout << "im here" << std::endl;
    served.seekg(0, std::ios::end);
    int file_size = served.tellg();
    served.seekg(0, std::ios::beg);
    char *buffer = new char[file_size + 1]();
    sprintf(buffer, "HTTP/1.1 201 Created\r\n");
    send((*_clientInfoIt)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send((*_clientInfoIt)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %d\r\n", file_size);
    send((*_clientInfoIt)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
    send((*_clientInfoIt)->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send((*_clientInfoIt)->socket, buffer, strlen(buffer), 0);
    served.read(buffer, file_size);
    send((*_clientInfoIt)->socket, buffer, strlen(buffer), 0);
    delete [] buffer;
}