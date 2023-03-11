# include "../Interfaces/POSTMethod.hpp"
# include "../errorsHandling/errorsHandling.hpp"

PostMethod::PostMethod(ServerConfiguration &serverConfiguration)
: _serverConfiguration(serverConfiguration), cgiContentLength(0)
{}


//void    PostMethod::_searchForCurrentLocation(ClientInfo *client) {
//    std::list<LocationBlockParse>::iterator beg = this->_serverConfiguration.Locations.begin();
//    for (; beg != this->_serverConfiguration.Locations.end(); beg++)
//    {
//        LocationBlockParse loc = *beg;
//        std::string res = loc.Location;
//        std::string path =  client->parsedRequest.requestDataMap["path"];
//        int len = path.length() - 1;
//        int index_last = len;
//        if (path[len] == '/')
//            len--;
//        bool is_file_last = 0, point = 0;
//        for (; len >= 0; len--) {
//            if (path[len] == '.')
//                point = 1;
//            if (path[len] == '/' && point) {
//                is_file_last = 1;
//                index_last = len;
//                break;
//            } else if (path[len] == '/' && !point) break;
//        }
//        if (res[res.length() - 1] != '/') res += '/';
//        std::string full_path = path.substr(0, index_last + 1);
//        if (!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
//        if (full_path != res) continue;
//        client->_currentLocation = beg;
////        std::list<std::pair<std::string, std::string> >::iterator CGIit = this->.CGI.begin();
////        for(std::list<> ; CGIit != client->_currentLocation.CGI.end(); CGIit++ ){
////                if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
////                    client->cgiContentLength =  "0";
////                    client->cgiContentType = "";
////                    client->CGIexecutedFile(currentPath, client, serverConfig, CGIit);
////                    return ;
////                }
////                else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
////                    // python cgi
////                }
////        }
//        return ;
//    }
//    client->_currentLocation = beg;
//}

bool    PostMethod::_isLocationSupportsUpload( ClientInfo *client ) {
    return (client->_currentLocation.UploadDirectoryPath.length());
}

void   PostMethod::startPostRequest(ClientInfo *client, bool isNotUpload)
{
        if(client->parsedRequest.isBoundaryExist == true) {
            client->parsedRequest._parsingMiniHeader();
            if(isNotUpload == 1)
                client->actionPath = client->servedFileName;
            client->servedFileName += client->parsedRequest.uploadFileName;
        }
        else {
            client->parsedRequest.uploadFileName = client->generateRandString() + get_real_format(client->parsedRequest.requestDataMap["Content-Type:"].c_str());
            if(isNotUpload == 1)
                client->actionPath = client->servedFileName;
            client->servedFileName += client->parsedRequest.uploadFileName;
        }
        client->postRequest->_preparingPostRequest(client);
}
void    PostMethod::handleFirstRead(ClientInfo *client) {
    if(this->_isLocationSupportsUpload(client))
        startPostRequest(client, 0);
    else
    {
        startPostRequest(client, 1);
        client->postLocationAbsence(this->_serverConfiguration);
    }
}

void PostMethod::_preparingPostRequest(ClientInfo *client) 
{
    int a = (client->parsedRequest. isBoundaryExist == true) ? 4 : 0;
    int b = (client->parsedRequest.isBoundaryExist == true) ? 0 : 4;
    client->requestBody.open(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
    client->requestBody.write(client->parsedRequest.requestHeader + client->parsedRequest.bodyIndex + b, client->parsedRequest.received - client->parsedRequest.newBodyIndex - a);
}




void PostMethod::_writeInTempFile(ClientInfo *client) {
    client->requestBody.write(client->parsedRequest.requestHeader, client->parsedRequest.receivedBytes);
    if (client->requestBody.fail())
        throw (std::runtime_error("Error Occurred In writeIntTempFile"));
}

void PostMethod::_receiveFromClient(ClientInfo *client){
    client->parsedRequest.bytesToReceive = (client->parsedRequest.received + MAX_REQUEST_SIZE < client->parsedRequest.contentLength) ? MAX_REQUEST_SIZE : client->parsedRequest.contentLength - client->parsedRequest.received;
    if ((client->parsedRequest.receivedBytes = recv(client->socket, client->parsedRequest.requestHeader, client->parsedRequest.bytesToReceive, 0)) == -1)
        throw (std::runtime_error("Error Occurred In ReceiveFromClient"));
    client->parsedRequest.received += client->parsedRequest.receivedBytes;
    client->parsedRequest.requestHeader[client->parsedRequest.receivedBytes] = 0;
}

void PostMethod::receiveTheBody(ClientInfo *client){
    this->_receiveFromClient(client);
    this->_writeInTempFile(client);
    if (client->parsedRequest.received == client->parsedRequest.contentLength)
        client->preparingMovingTempFile(client);
};


void  PostMethod::successfulPostRequest(ClientInfo *client){
    std::string path = "uploadSuccess.html";
    client->isCreated = remove((TMP_FOLDER_PATH + client->parsedRequest.uploadFileName).c_str());
    if(client->served.is_open())
        client->served.close();
    client->served.open(path);
    client->served.seekg(0, std::ios::end);
    int file_size = client->served.tellg();
    client->served.seekg(0, std::ios::beg);

    client->headerToBeSent += "HTTP/1.1 201 Created\r\n"
    + std::string("Connection: close\r\n")
    + std::string("Content-Length: ")
    + std::to_string(file_size)
    + "\r\n"
    +  std::string("Content-Type: ")
    +  get_mime_format(path.c_str())
    + "\r\n\r\n" ;
    client->isSendingHeader = true;
}
