# include "../Interfaces/POSTMethod.hpp"
# include "../errorsHandling/errorsHandling.hpp"

PostMethod::PostMethod(ServerConfiguration &serverConfiguration)
: _serverConfiguration(serverConfiguration), totalTempFileSize(0), toWrite(0)
{}


void    PostMethod::_searchForCurrentLocation(ClientInfo *client) {
    std::list<LocationBlockParse>::iterator beg = this->_serverConfiguration.Locations.begin();
    for (; beg != this->_serverConfiguration.Locations.end(); beg++)
    {
        LocationBlockParse loc = *beg;
        std::string res = loc.Location;
        std::string path =  client->parsedRequest.requestDataMap["path"];
        int len = path.length() - 1;
        int index_last = len;
        if (path[len] == '/')
            len--;
        bool is_file_last = 0, point = 0;
        for (; len >= 0; len--) {
            if (path[len] == '.')
                point = 1;
            if (path[len] == '/' && point) {
                is_file_last = 1;
                index_last = len;
                break;
            } else if (path[len] == '/' && !point) break;
        }
        if (res[res.length() - 1] != '/') res += '/';
        std::string full_path = path.substr(0, index_last + 1);
        if (!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
        if (full_path != res) continue;
        this->_currentLocation = beg;
        return ;
    }
    this->_currentLocation = beg;
}

bool    PostMethod::_isLocationSupportPost() {
    std::list<std::string>::iterator it = this->_currentLocation->allowedMethods.begin();
    while (it != this->_currentLocation->allowedMethods.end()){
        if (*it == "POST")
            return (true);
        it++;
    }
    return (false);
}

void    PostMethod::handleFirstRead(ClientInfo *client) {
     this->_searchForCurrentLocation(client);
     if(this->_currentLocation == this->_serverConfiguration.Locations.end()){
         error_404(client);
         throw std::runtime_error("Location not found");
     }
     if (!this->_isLocationSupportPost())
         throw (std::runtime_error("Post Method is not supported !!")); // this line was just added and need to be tested.....
     if(this->_currentLocation->UploadDirectoryPath.length()){
         client->parsedRequest._parsingMiniHeader();
         client->postRequest->_preparingPostRequest(client);
         client->postRequest->_isValidPostRequest(client);
     }
     else{

     }
}


void PostMethod::_preparingPostRequest(ClientInfo *client) {
    client->requestBody.open(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
//    if (!client->requestBody.is_open()) // this gives a problem when trying to upload yt.mp4 video.
//        throw (std::runtime_error(TMP_FOLDER_COULDNT_OPEN));
    client->requestBody.write(client->parsedRequest.requestHeader + client->parsedRequest.bodyIndex, client->parsedRequest.received - client->parsedRequest.newBodyIndex - 4);
}


void PostMethod::_isValidPostRequest(ClientInfo *client) {
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


void PostMethod::_writeInTempFile(ClientInfo *client) {
    client->requestBody.write(client->parsedRequest.requestHeader, client->parsedRequest.receivedBytes);
    if (client->requestBody.fail())
        throw (std::runtime_error("Error Occurred In writeIntTempFile\n"));
}

void PostMethod::_receiveFromClient(ClientInfo *client){
    client->parsedRequest.bytesToReceive = (client->parsedRequest.received + MAX_REQUEST_SIZE < client->parsedRequest.contentLength) ? MAX_REQUEST_SIZE : client->parsedRequest.contentLength - client->parsedRequest.received;
    if ((client->parsedRequest.receivedBytes = recv(client->socket, client->parsedRequest.requestHeader, client->parsedRequest.bytesToReceive, 0)) == -1)
        throw (std::runtime_error("Error Occurred In ReceiveFromClient\n"));
    client->parsedRequest.received += client->parsedRequest.receivedBytes;
    client->parsedRequest.requestHeader[client->parsedRequest.receivedBytes] = 0;
}

void PostMethod::receiveTheBody(ClientInfo *client){
    this->_receiveFromClient(client);
    this->_writeInTempFile(client);
    if (client->parsedRequest.received == client->parsedRequest.contentLength)
        this->preparingMovingTempFile(client);
};


void PostMethod::preparingMovingTempFile(ClientInfo *client) {
    int i = 0;
    this->totalTempFileSize = client->parsedRequest.received - client->parsedRequest.boundarySize - client->parsedRequest.newBodyIndex - 8;
    this->toWrite = 0;
    client->requestBody.close();
    struct stat st;
    if (!(stat(this->_currentLocation->UploadDirectoryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))) {
        i = mkdir(this->_currentLocation->UploadDirectoryPath.c_str(), O_CREAT | S_IRWXU | S_IRWXU | S_IRWXO);
    }
    this->sourceFile.open(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
    std::string filePath = this->_currentLocation->UploadDirectoryPath + "/" + client->parsedRequest.uploadFileName;
    this->destinationFile.open(filePath, std::ios::binary);
    if (i == -1 || this->destinationFile.fail())
        throw (std::runtime_error("Error Occurred in preparingMovingTempFile"));
}

void PostMethod::writeToUploadedFile()
{
    this->toWrite = (this->totalTempFileSize > 1024) ? 1024 : this->totalTempFileSize;
    char buffer[this->toWrite + 1];
    this->sourceFile.read(buffer, this->toWrite);
    buffer[this->toWrite] = 0;
    this->destinationFile.write(buffer, this->toWrite);
    if (this->sourceFile.fail() || this->destinationFile.fail())
        throw (std::runtime_error("Error Occurred in writeToUploadedFile"));
    this->totalTempFileSize -= this->toWrite;
}

void  PostMethod::successfulPostRequest(ClientInfo *client){
    this->sourceFile.close();
    this->destinationFile.close();
    std::string path = "uploadSuccess.html";
    int i = remove((TMP_FOLDER_PATH + client->parsedRequest.uploadFileName).c_str());
    if(client->served.is_open())
        client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);
    std::string error_header = "";
    error_header += "HTTP/1.1 201 Created\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size)
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    if (send(client->socket, error_header.c_str(), error_header.length(), 0) == -1 || i == -1)
        throw (std::runtime_error("Error Occurred in successfulPostRequest"));
}