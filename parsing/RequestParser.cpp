#include "RequestParser.hpp"

void	ParsingRequest::parsingRequestFirstLine(std::string line, client_info *client){
	std::stringstream str(line);
    std::string word;
    str >> word;
    client->request_data["method"] = word;
    str >> word;
    if(isUriTooLong(word)) {

    }
    client->request_data["path"] = word;
    str >> word;
    client->request_data["httpVersion"] = word;
}
void	ParsingRequest::parsingRequest(std::string line, client_info *client){
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
        client->contentLength = atoi(last.c_str());
    client->request_data[save] = last;
}
void	ParsingRequest::parse(client_info *client){
	int	i = 0;
	std::string	headerPart(client->requestHeader), line, bodyPart(headerPart);
    headerPart = headerPart.substr(0, client->bodyIndex);
    std::size_t found = headerPart.find("\r\n");
    while(found != std::string::npos)
    {
      line = headerPart.substr(0, found);
      if (i == 0)
      {
          parsingRequestFirstLine(line, client);
          i++;
      }
      else
          parsingRequest(line, client);
      headerPart = headerPart.substr(found + 2);
      found = headerPart.find("\r\n");
    }
	headerPart = headerPart.substr(found + 1);
    found = headerPart.find("\r\n");
    line = headerPart.substr(0, found);
    parsingRequest(line, client);
}