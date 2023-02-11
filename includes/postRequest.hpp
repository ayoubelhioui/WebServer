#ifndef _POST_REQUEST_HPP_
#define _POST_REQUEST_HPP_


struct postRequestStruct{
    client_info *client;
    std::list<client_info *>::iterator clientDataIterator;
    std::list<client_info *> clientData;
    std::map<std::string, std::string> &requestData;
    Parsing &configFileData;
    int bodyIndex;
    postRequestStruct(client_info *client, std::list<client_info *>::iterator clientDataIterator, std::list<client_info *> clientData,
                      std::map<std::string, std::string> &requestData, Parsing &configFileData, int bodyIndex, int requestSize) : client(client),
                      clientDataIterator(clientDataIterator), clientData(clientData), requestData(requestData), configFileData(configFileData), bodyIndex(bodyIndex){};
};

bool    isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData);
void    requestBodyTooLong(client_info *client);
bool    isBodySizeBigger(Parsing &servers, int bodySize, client_info *client);
int     ret_index(char *str);
void    handlingPostRequest(postRequestStruct &postRequest, int &boundarySize);
void    searchForBoundary(std::map<std::string, std::string> &requestData, int &bodyIndex, char *clientRequest, int &boundarySize);

#endif