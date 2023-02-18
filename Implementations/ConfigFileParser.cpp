# include "../Interfaces/ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser ( std::string &configFilePath )
	: _configFilePath(configFilePath) {}

ConfigFileParser::ConfigFileParser ( void )
{
	
}

ConfigFileParser::~ConfigFileParser ( void )
{
	
}

// ConfigFileParser	&ConfigFileParser::operator= ( const ConfigFileParser &obj )
// {
// 	if (this == &obj)
// 		return (*this);
// 	return (*this);
// }

// ConfigFileParser::ConfigFileParser ( const ConfigFileParser &obj )
// {
// 	*this = obj;
// }
void ConfigFileParser::readingDataFromFile( void )
{
    std::ifstream configFile(_configFilePath);
    std::string enteredData;

    if (!configFile.is_open())
        errorPrinting(CONFIG_FILE_COULDNT_OPEN_MSG);
    while (std::getline(configFile, enteredData))
        configFileLines.push_back(enteredData);
}


void ServerConfiguration::startParsingFile( void )
{
    std::list<std::string>::iterator it = configFileInfo.begin();
    while (*it == "\n") it++;
    while (it != configFileInfo.end())
    {
        ServerConfiguration newParseNode;
        while ((*it).find("location") == std::string::npos)
            newParseNode.fillingDataFirstPart(*it++);
        while((*it).find("location") != std::string::npos)
        {
            locationBlockParse newLocationNode;
            newLocationNode.locationParse(it);
            if((*it).find("location") == std::string::npos && *it != "}")
                errorPrinting("error : You didn't close the server or may have other problems");
            newParseNode.Locations.push_back(newLocationNode);
        }
        configFileList.push_back(newParseNode);
        if (*it == "}")
            it++;
        if (it == configFileInfo.end())
            return ;
        while (*it == "")
        {
            std::cout << "hello world" << std::endl;
            it++;
            if (it == configFileInfo.end())
                return ;
        }
    }
}


void ServerConfiguration::printingParsingData(std::list<ServerConfiguration> &parsingData)
{
    for(std::list<ServerConfiguration>::iterator it = parsingData.begin(); it != parsingData.end(); it++)
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


void	ConfigFileParser::parseConfigFile ( void )
{
	ConfigFileParser::readingDataFromFile();
    ConfigFileParser::startParsingFile();
    ConfigFileParser::printingParsingData();
}