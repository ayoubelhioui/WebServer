#include "../Interfaces/ServerConfiguration.hpp"

ServerConfiguration::ServerConfiguration() : serverHost("0.0.0.0"), serverPort("80"), clientBodyLimit(UINT_MAX)
{
    this->errorInfo["400"] = "./public/htmlErrorPages/error400.html";
    this->errorInfo["500"] = "./public/htmlErrorPages/error500.html";
    this->errorInfo["405"] = "./public/htmlErrorPages/error405.html";
    this->errorInfo["501"] = "./public/htmlErrorPages/error501.html";
    this->errorInfo["413"] = "./public/htmlErrorPages/error413.html";
    this->errorInfo["404"] = "./public/htmlErrorPages/error404.html";
    this->errorInfo["414"] = "./public/htmlErrorPages/error414.html";
    this->errorInfo["403"] = "./public/htmlErrorPages/error403.html";
}


void ServerConfiguration::clientBodySizeKeywordFound(std::vector<std::string> &vec){
    if (vec.size() != 2 || !isValidNumber(vec[1]))
        errorPrinting("error in client body size");
    this->clientBodyLimit = atoi(vec[1].c_str());
};

void ServerConfiguration::fillingDataFirstPart(std::string &data){
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

void ServerConfiguration::listenKeywordFound(std::vector<std::string> &vec){
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

void ServerConfiguration::serverNameKeywordFound(std::vector<std::string> &vec){
    if (vec.size() < 1)
        errorPrinting(MISSING_SERVER_NAME);
    for (size_t i = 1; i < vec.size(); i++)
        this->serverName.push_back(vec[i]);
}

void ServerConfiguration::errorPageKeywordFound(std::vector<std::string> &vec){
    if (vec.size() != 3 || !isValidNumber(vec[1]))
        errorPrinting(ERROR_PAGE_ERROR_MSG);
    std::ifstream errorPageFile(vec[2]);
    if (!errorPageFile.is_open())
        return ;
    else
        this->errorInfo[vec[1]] = vec[2];
}

// ServerConfiguration	&ServerConfiguration::operator= ( const ServerConfiguration &obj )
// {
// 	if (this == &obj)
// 		return (*this);
// 	this->serverHost = obj.serverHost;
// 	this->serverPort = obj.serverPort;
// 	this->clientBodyLimit = obj.clientBodyLimit;

// 	return (*this);
// }

// ServerConfiguration::ServerConfiguration ( const ServerConfiguration &obj )
// {
// 	*this = obj;
// }

void	ServerConfiguration::printServerConfiguration ( void )
{
	 std::cout << "***********************************  CONFIG FILE DATA FOR EVERY SERVER *******************************" << std::endl;
        std::cout << "SERVER HOST : " <<  this->serverHost << std::endl;
        std::cout << "SERVER PORT : " <<  this->serverPort << std::endl;
        std::cout << "SERVER BODY LIMIT : " <<  this->clientBodyLimit << std::endl;
        std::cout << "SERVER NAMES : ";
        for (std::list<std::string>::iterator it1 = this->serverName.begin(); it1 != this->serverName.end(); it1++)
            std::cout << *it1 << " ";
        std::cout << std::endl;
        std::cout << "SERVER ERROR PAGES : ";
        for (std::map<std::string, std::string>::iterator it2 = this->errorInfo.begin(); it2 != this->errorInfo.end(); it2++)
            std::cout << (*it2).first << " " << (*it2).second << std::endl;
        for (std::list<LocationBlockParse>::iterator it4 = this->Locations.begin(); it4 != this->Locations.end(); it4++)
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