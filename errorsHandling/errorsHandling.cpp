#include "../Interfaces/ConfigFileParser.hpp"

bool    isBodySizeBigger(Parsing &servers, int bodySize, client_info *client) // STATUS CODE : 403 REQUEST ENTITY TOO LARGE
{
    return (bodySize > servers.clientBodyLimit);
}

bool    isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData) //STATUS CODE: 501 NOT IMPLEMENTED
{
    std::map<std::string, std::string>::iterator Transfer_Encoding = requestData.find("Transfer-Encoding:");
    std::map<std::string, std::string>::iterator Content_length = requestData.find("Content-Length:");
    return (Content_length == requestData.end() and Transfer_Encoding != requestData.end() and Transfer_Encoding->second != "chunked");
}

bool    isNotValidPostRequest(std::map<std::string, std::string> &requestData) // this function should be called when we find a POST request // STATS CODE: 400 BAD REQUEST
{
     std::map<std::string, std::string>::iterator it = requestData.find("Transfer-Encoding:");
     std::map<std::string, std::string>::iterator it1 = requestData.find("Content-Length:");
//     std::cout << (it == requestData.end() and it1 == requestData.end()) << std::endl;
//     std::cout << "transfer : " << it->second << std::endl;
//     std::cout << "content : " << it1->second << std::endl;
     return (it == requestData.end() and it1 == requestData.end());
}

bool isUriTooLong(std::string &Uri)
{
    return (Uri.length() > 2048);
}


