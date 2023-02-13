#ifndef _POST_REQUEST_HPP_
#define _POST_REQUEST_HPP_


class postRequestExceptions : std::runtime_error{
    public:
        postRequestExceptions(const std::string &errorMessage) : std::runtime_error(errorMessage){};
};

struct postRequestStruct{
    client_info *client;
    std::list<client_info *>::iterator clientDataIterator;
    std::list<client_info *> clientData;
    Parsing &configFileData;
    postRequestStruct(client_info *client, std::list<client_info *>::iterator clientDataIterator, std::list<client_info *> clientData, Parsing &configFileData) :
    client(client),
                      clientDataIterator(clientDataIterator), clientData(clientData), configFileData(configFileData){};
};

bool    isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData);
void    requestBodyTooLong(client_info *client);
bool    isBodySizeBigger(Parsing &servers, int bodySize, client_info *client);
int     ret_index(char *str);
void    searchForBoundary(client_info *client);
void    isValidPostRequest(postRequestStruct &postRequest);
void    succesfulPostRequest(std::list<client_info *>::iterator &clientDataIterator, std::list<client_info *> &clientData, client_info *client);

#endif