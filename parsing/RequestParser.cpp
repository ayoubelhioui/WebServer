#include "../Interfaces/RequestParser.hpp"

ParsingRequest::ParsingRequest() : bodyIndex(0), bytesToReceive(0), contentLength(0), receivedBytes(0)
{
    requestHeader = new char[2001]();
}

ParsingRequest::~ParsingRequest()
{
    delete [] requestHeader;
}

void	ParsingRequest::parsingRequestFirstLine(std::string line){
	std::stringstream str(line);
    std::string word;
    str >> word;
    request_data["method"] = word;
    str >> word;
    request_data["path"] = word;
    str >> word;
    request_data["httpVersion"] = word;
}
void	ParsingRequest::parsingRequest(std::string line){
	std::stringstream str(line);
    std::string word;
    str >> word;
    std::string save = word;
    std::string last;
    while (str >> word)
    {
        last += " ";
        last += word;
    }
    last.erase(0, 1);
    if (save == "Content-Length:")
        contentLength = atoi(last.c_str());
    request_data[save] = last;
}
void	ParsingRequest::parse(){
	int	i = 0;
	std::string	headerPart(requestHeader), line, bodyPart(headerPart);
    headerPart = headerPart.substr(0, bodyIndex);
    std::size_t found = headerPart.find("\r\n");
    while(found != std::string::npos)
    {
      line = headerPart.substr(0, found);
      if (i == 0)
      {
          parsingRequestFirstLine(line);
          i++;
      }
      else
          parsingRequest(line);
      headerPart = headerPart.substr(found + 2);
      found = headerPart.find("\r\n");
    }
	headerPart = headerPart.substr(found + 1);
    found = headerPart.find("\r\n");
    line = headerPart.substr(0, found);
    parsingRequest(line);
}

void    ParsingRequest::receiveFirstTime(int socket){
    receivedBytes = recv(socket, requestHeader, MAX_REQUEST_SIZE, 0);
    requestHeader[receivedBytes] = 0;
    bytesToReceive += receivedBytes;
    bodyIndex = retIndex(requestHeader);
}

int     ParsingRequest::retIndex(char *header){
    for(int i = 0; requestHeader[i]; i++){
      if(!strncmp(&requestHeader[i], "\r\n\r\n", 4))
          return i;
    }
    return -1;
}