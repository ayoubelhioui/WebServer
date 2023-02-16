#include "../Interfaces/configFileParse.hpp"

configFileParse::configFileParse() : serverHost("0.0.0.0"), serverPort("80"), clientBodyLimit(UINT_MAX), isClosed(false){}

locationBlockParse::locationBlockParse() : Root("RootFiles"), isDirectoryListingOn(0) {}

//client_info::client_info() : received(0), isFirstRead(false), bodyFirstRead(false), boundarySize(0), bytesToReceive(0) {}
//
//client_info::~client_info() {}

void configFileParse::clientBodySizeKeywordFound(std::vector<std::string> &vec){
    if (vec.size() != 2 || !isValidNumber(vec[1]))
        errorPrinting("error in client body size");
    this->clientBodyLimit = atoi(vec[1].c_str());
};
void configFileParse::fillingDataFirstPart(std::string &data){
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

void configFileParse::listenKeywordFound(std::vector<std::string> &vec){
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

void configFileParse::serverNameKeywordFound(std::vector<std::string> &vec){
    if (vec.size() < 1)
        errorPrinting(MISSING_SERVER_NAME);
    for (size_t i = 1; i < vec.size(); i++)
        this->serverName.push_back(vec[i]);
}

void configFileParse::errorPageKeywordFound(std::vector<std::string> &vec){
    if (vec.size() != 3 || !isValidNumber(vec[1]))
        errorPrinting(ERROR_PAGE_ERROR_MSG);
    std::ifstream errorPageFile(vec[2]);
    if (!errorPageFile.is_open())
        errorPrinting(ERROR_PAGE_FILE_NOT_FOUND);
    this->errorInfo.push_back(std::make_pair(atoi(vec[1].c_str()), vec[2].c_str()));
}

void configFileParse::printingParsingData(std::list<configFileParse> &parsingData)
{
    for(std::list<configFileParse>::iterator it = parsingData.begin(); it != parsingData.end(); it++)
    {
        std::cout << "***********************************  CONFIG FILE DATA FOR EVERY SERVER *******************************" << std::endl;
        std::cout << "SERVER HOST : " <<  it->serverHost << std::endl;
        std::cout << "SERVER PORT : " <<  it->serverPort << std::endl;
        std::cout << "SERVER BODY LIMIT : " <<  it->clientBodyLimit << std::endl;
        std::cout << "SERVER NAMES : ";
        for (std::list<std::string>::iterator it1 = it->serverName.begin(); it1 != it->serverName.end(); it1++)
            std::cout << *it1 << " ";
        std::cout << std::endl;
        std::cout << "SERVER ERROR PAGES : ";
        for (std::list<std::pair<int, std::string > >::iterator it2 = it->errorInfo.begin(); it2 != it->errorInfo.end(); it2++)
            std::cout << (*it2).first << " " << (*it2).second << std::endl;
        for (std::list<locationBlockParse>::iterator it4 = it->Locations.begin(); it4 != it->Locations.end(); it4++)
        {
            std::cout << "direc path " << it4->UploadDirectoryPath << std::endl;
            std::cout << "root is " << it4->Root << std::endl;
            std::cout << "redirect is " << it4->Redirection << std::endl;
            std::cout << "location path " << it4->Location << std::endl;
            std::cout << "direct listing " << it4->isDirectoryListingOn << std::endl;
            std::cout << "ALLOWED METHODS ";
            std::list<std::string>::iterator it1;
            for(it1 = it4->allowedMethods.begin(); it1 != it4->allowedMethods.end(); it1++){
                std::cout << *it1 << " ";
            }
            std::cout << "\n";
            std::cout << "index files :";
            std::list<std::string>::iterator it2;
            for(it2 = it4->indexFiles.begin(); it2 != it4->indexFiles.end(); it2++){
                std::cout << *it2 << " ";
            }
            std::cout << "\n";
            std::cout << "CGIS DATA : ";
            std::list<std::pair<std::string, std::string> >::iterator it3;
            for(it3 = it4->CGI.begin(); it3 != it4->CGI.end(); it3++){
                std::cout << "first part " << it3->first << " ";
                std::cout << "second part " << it3->second << std::endl;
            }
        }
    }
}

void configFileParse::startParsingFile(std::list<std::string> &configFileInfo, std::list<configFileParse> &configFileList)
{
    std::list<std::string>::iterator it = configFileInfo.begin();
    while (*it == "\n") it++;
    while (it != configFileInfo.end())
    {
        std::cout << "im here" << std::endl;
        configFileParse newParseNode;
        while ((*it).find("location") == std::string::npos)
            newParseNode.fillingDataFirstPart(*it++);
        while((*it).find("location") != std::string::npos)
        {
            locationBlockParse newLocationNode;
            newLocationNode.locationParse(it);
            if((*it).find("location") == std::string::npos
            && *it != "}") {
                errorPrinting("error : You didn't close the server or may have other problems");
            }
            newParseNode.Locations.push_back(newLocationNode);
        }
        configFileList.push_back(newParseNode);
        if (*it == "}")
            it++;
        while (*it == "")
            it++;
    }
}

void configFileParse::readingDataFromFile(std::list<std::string> &configFileInfo, std::string &configFilePath){
    std::ifstream configFile(configFilePath);
    std::string enteredData;
    if (!configFile.is_open())
        errorPrinting(CONFIG_FILE_COULDNT_OPEN_MSG);
    while (std::getline(configFile, enteredData))
        configFileInfo.push_back(enteredData);
}