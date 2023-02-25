# include "../Interfaces/POSTMethod.hpp"
# include "../errorsHandling/errorsHandling.hpp"



PostMethodExceptions::PostMethodExceptions(const std::string &errorMessage) : std::runtime_error(errorMessage) { };

PostMethod::PostMethod(ServerConfiguration &serverConfiguration)
: _serverConfiguration(serverConfiguration), totalTempFileSize(0), toWrite(0)
{}


void PostMethod::handleFirstRead(ClientInfo *client) {
     client->postRequest->_searchingForUploadFolder(client);
     client->parsedRequest._parsingMiniHeader();
     client->postRequest->_preparingPostRequest(client);
     client->postRequest->_isValidPostRequest(client);
}
void PostMethod::_preparingPostRequest(ClientInfo *client) {
    client->requestBody.open(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
    if (!client->requestBody.is_open())
        throw (std::runtime_error(TMP_FOLDER_COULDNT_OPEN));
    client->requestBody.write(client->parsedRequest.requestHeader + client->parsedRequest.bodyIndex, client->parsedRequest.received - client->parsedRequest.newBodyIndex - 4);
}

void PostMethod::_searchingForUploadFolder(ClientInfo *client) {
    std::string path = client->parsedRequest.requestDataMap["path"];
	for (std::list<LocationBlockParse>::iterator beg = serverConfig.Locations.begin(); beg != serverConfig.Locations.end(); beg++){
		LocationBlockParse loc = *beg;
		std::string res = loc.Location;
		int len = path.length() - 1;
		int	index_last = len;
		if(path[len] == '/')
			len--;
		bool is_file_last = 0, point = 0;
		for(; len >= 0; len--){
			if(path[len] == '.')
				point = 1;
			if(path[len] == '/' && point){
				is_file_last = 1;
				index_last = len;
				break;
			}
			else if (path[len] == '/' && !point) break;
		}
		if(res[res.length() - 1] != '/') res += '/';
		std::string full_path = path.substr(0, index_last + 1);
		if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
		if(full_path != res) continue;
		if(loc.isDirectoryListingOn && !is_file_last){
            std::string root = loc.Root;
            if(root[root.length() - 1] != '/') root += '/';
            if(root[0] != '.') root = '.' + root;
            if(full_path[0] != '.') full_path = '.' + full_path;
            return directoryListing(root, full_path);
        }
        else{
		    std::string file = path.substr(index_last + 1);
		    if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
		    if(is_file_last && file[file.length() - 1] == '/') file.erase(file.length() - 1);
		    std::string root = loc.Root;
		    if(root[root.length() - 1] != '/') root += '/';
		    if(file == ""){
		    	for(std::list<std::string>::iterator index_it = loc.indexFiles.begin(); index_it != loc.indexFiles.end(); index_it++)
		    	{
		    		std::string final_path = root + (*index_it);
		    		if(final_path[0] == '/') final_path = '.' + final_path;
		    		std::ifstream check_file(final_path, std::ios::binary);
		    		if(check_file){
					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
//					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
//                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
//                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
//                            return CGIexecutedFile(final_path, client->parsedData.queryString, serverConfig);
//                        }
//                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
//                            // python cgi
//                        }
//                    }
					return final_path;
				}
		    		else ;
		    	}
		    }
		    else{
		    	std::string final_path = root + file;
		    	if(final_path[0] == '/') final_path = '.' + final_path;
		    	std::ifstream check_file(final_path, std::ios::binary);
		    	if(check_file){
//					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
//					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
//                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
//                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
//                            return CGIexecutedFile(final_path, parsedData.queryString, serverConfig);
//                        }
//                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
//                            // python cgi
//                        }
                    }
					return final_path;
				}
		    	else ;
		    }
        }
	}
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
}

void PostMethod::_receiveFromClient(ClientInfo *client){
    client->parsedRequest.bytesToReceive = (client->parsedRequest.received + MAX_REQUEST_SIZE < client->parsedRequest.contentLength) ? MAX_REQUEST_SIZE : client->parsedRequest.contentLength - client->parsedRequest.received;
    client->parsedRequest.receivedBytes = recv(client->socket, client->parsedRequest.requestHeader, client->parsedRequest.bytesToReceive, 0);
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
    this->totalTempFileSize = client->parsedRequest.received - client->parsedRequest.boundarySize - client->parsedRequest.newBodyIndex - 8;
    this->toWrite = 0;
     client->requestBody.close();
     this->sourceFile.open(TMP_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
     this->destinationFile.open(UPLOADS_FOLDER_PATH + client->parsedRequest.uploadFileName, std::ios::binary);
     if (!destinationFile.is_open() || !sourceFile.is_open())
        throw (std::runtime_error("couldn't Open" + client->parsedRequest.uploadFileName));

}
void PostMethod::writeToUploadedFile()
{
    this->toWrite = (this->totalTempFileSize > 1024) ? 1024 : this->totalTempFileSize;
    char buffer[this->toWrite + 1];
    this->sourceFile.read(buffer, this->toWrite);
    buffer[this->toWrite] = 0;
    this->destinationFile.write(buffer, this->toWrite);
    this->totalTempFileSize -= this->toWrite;
}


void  PostMethod::successfulPostRequest(ClientInfo *client){
    this->sourceFile.close();
    this->destinationFile.close();
    std::string path = "uploadSuccess.html";
    std::ifstream served(path);
    if (!served.is_open())
        throw (std::runtime_error(UPLOAD_SUCCESS_FILE_PROBLEM));
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
    delete this;
}