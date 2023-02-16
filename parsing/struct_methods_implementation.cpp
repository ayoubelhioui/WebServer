#include "parsing.hpp"

client_info::client_info() : received(0), isFirstRead(false), bodyFirstRead(false), boundarySize(0), bytesToReceive(0)
{
}

client_info::~client_info()
{
}

Parsing::Parsing() : serverHost("0.0.0.0"), serverPort("80"), clientBodyLimit(UINT_MAX), isClosed(false){}

locationBlock::locationBlock() : Root("RootFiles"), isDirectoryListingOn(0) {}

void Parsing::clientBodySizeKeywordFound(std::vector<std::string> &vec){
    if (vec.size() != 2 || !isValidNumber(vec[1]))
        errorPrinting("error in client body size");
    this->clientBodyLimit = atoi(vec[1].c_str());
};
void Parsing::fillingDataFirstPart(std::string &data){
    if (data.length() == 0)
        return ;
    std::stringstream str(data);
    std::string word;
    std::vector<std::string> vec;
    while (str >> word)
        vec.push_back(word);
    if (vec[0] == "#" || vec[0][0] == '#')
    {
        if ((vec[0] == "#" && vec[1] == SERVER_KEYWORD) || vec[0] == "#server")
            errorPrinting(LISTEN_ERROR_MSG);
        return ;
    }
    if (vec[0] == LISTEN_KEYWORD)
        listenKeywordFound(vec);
    else if (vec[0] == SERVER_KEYWORD){
        if (vec.size() != 2 || vec[0] != SERVER_KEYWORD || vec[1] != "{") {
            errorPrinting(SERVER_KEYWORD_MSG);
        }
    }
    else if (vec[0] == SERVER_NAME_KEYWORD)
        serverNameKeywordFound(vec);
    else if (vec[0] == ERROR_PAGE_KEYWORD)
        errorPageKeywordFound(vec);
    else if (vec[0] == MAX_CLIENT_BODY_SIZE_KEYWORD)
        clientBodySizeKeywordFound(vec);
    else
        std::cout << vec[0], errorPrinting(INVALID_KEYWORD);
}

void Parsing::listenKeywordFound(std::vector<std::string> &vec){
    std::string port;
    if (vec.size() != 2)
        errorPrinting(LISTEN_ERROR_MSG);
    int index = vec[1].find(':');
    if (index == 0)
        errorPrinting(LISTEN_ERROR_MSG);
    port = vec[1].substr(index + 1, vec[1].length());
    this->serverHost = vec[1].substr(0, index);
    if (!isValidNumber(port))
        errorPrinting(INVALID_PORT_MSG);
    this->serverPort = port.c_str();
}

void Parsing::serverNameKeywordFound(std::vector<std::string> &vec){
    if (vec.size() < 1)
        errorPrinting(MISSING_SERVER_NAME);
    for (size_t i = 1; i < vec.size(); i++)
        this->serverName.push_back(vec[i]);
}

void Parsing::errorPageKeywordFound(std::vector<std::string> &vec){
    if (vec.size() != 3 || !isValidNumber(vec[1]))
        errorPrinting(ERROR_PAGE_ERROR_MSG);
    std::ifstream errorPageFile(vec[2]);
    if (!errorPageFile.is_open())
        errorPrinting(ERROR_PAGE_FILE_NOT_FOUND);
    this->errorInfo.push_back(std::make_pair(atoi(vec[1].c_str()), vec[2].c_str()));
}

