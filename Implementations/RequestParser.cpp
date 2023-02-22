#include "../Interfaces/RequestParser.hpp"

ParsingRequest::ParsingRequest() : bodyIndex(0), bytesToReceive(0), contentLength(0)
, receivedBytes(0), received(0), boundarySize(0), newBodyIndex(0)
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
    requestDataMap["method"] = word;
    str >> word;
    requestDataMap["path"] = word;
    str >> word;
    requestDataMap["httpVersion"] = word;
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
    requestDataMap[save] = last;
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
    this->receivedBytes = recv(socket, this->requestHeader, MAX_REQUEST_SIZE, 0);
    this->requestHeader[this->receivedBytes] = 0;
    this->received += this->receivedBytes;
    this->bodyIndex = retIndex(this->requestHeader);
    this->received -= this->bodyIndex;
}

int     ParsingRequest::retIndex(char *requestHeader){
    for(int i = 0; requestHeader[i]; i++){
      if(!strncmp(&requestHeader[i], "\r\n\r\n", 4))
          return i;
    }
    return -1;
}


void ParsingRequest::gettingNewBodyIndex(std::string &boundarySavior)
{
    this->newBodyIndex = retIndex(requestHeader + bodyIndex + 4);
    this->received -= newBodyIndex;
    this->bodyIndex += newBodyIndex + 8;
    this->boundarySize = boundarySavior.length() - boundarySavior.find("=") + 3;
}

void ParsingRequest::gettingFileName(std::string &newString)
{
    int newContentIndex = newString.find("filename=");
    int DoubleQuotePosition = newString.find("\"", newContentIndex + 11);
    uploadFileName = newString.substr(newContentIndex + 10, DoubleQuotePosition - newContentIndex - 10);
}


void ParsingRequest::parsingMiniHeader( void )
{
    std::map<std::string, std::string>::iterator content = requestDataMap.find("Content-Type:");
    std::string boundarySavior = content->second;
    std::string newString(requestHeader + bodyIndex);
    gettingFileName(newString);
    gettingNewBodyIndex(boundarySavior);
    this->contentLength = this->contentLength - this->newBodyIndex - this->boundarySize - 8;
}