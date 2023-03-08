# include "../Interfaces/ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser ( std::string &configFilePath )
	: _configFilePath(configFilePath)
{}

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
void ConfigFileParser::_readingDataFromFile( void )
{
    std::ifstream configFile(_configFilePath);
    std::string enteredData;

    if (!configFile.is_open())
        errorPrinting(CONFIG_FILE_COULDNT_OPEN_MSG);
    while (std::getline(configFile, enteredData))
        _configFileLines.push_back(enteredData);
}


void	ConfigFileParser::_startParsingFile( void )
{
    std::list<std::string>::iterator configFileLinesIt;

	configFileLinesIt = this->_configFileLines.begin();
    while (*configFileLinesIt == "\n") configFileLinesIt++;
    while (configFileLinesIt != _configFileLines.end())
    {
        ServerConfiguration newParseNode;
        while ((*configFileLinesIt).find("location") == std::string::npos)
            newParseNode.fillingDataFirstPart(*configFileLinesIt++);
        while((*configFileLinesIt).find("location") != std::string::npos)
        {
            LocationBlockParse newLocationNode;
            newLocationNode.locationParse(configFileLinesIt);
            if((*configFileLinesIt).find("location") == std::string::npos && *configFileLinesIt != "}")
                errorPrinting("error : You didn't close the server or may have other problems");
            newParseNode.Locations.push_back(newLocationNode);
        }
        this->listOfServersConfiguration.push_back(newParseNode);
        if (*configFileLinesIt == "}")
            configFileLinesIt++;
        if (configFileLinesIt == _configFileLines.end())
            return ;
        while (*configFileLinesIt == "")
        {
            configFileLinesIt++;
            if (configFileLinesIt == _configFileLines.end())
                return ;
        }
    }
}


void ConfigFileParser::printingParsingData( void )
{
    for(std::list<ServerConfiguration>::iterator it = listOfServersConfiguration.begin(); it != listOfServersConfiguration.end(); it++)
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
        for (std::map<std::string, std::string>::iterator it2 = it->errorInfo.begin(); it2 != it->errorInfo.end(); it2++)
            std::cout << (*it2).first << " " << (*it2).second << std::endl;
        for (std::list<LocationBlockParse>::iterator it4 = it->Locations.begin(); it4 != it->Locations.end(); it4++)
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
	this->_readingDataFromFile();
    this->_startParsingFile();
}