// # include "webserver.hpp"
# include "Interfaces/ConfigFileParser.hpp"
# include "Interfaces/MultiHttpServer.hpp"

/*
int _main()
{



    
	std::list<ConfigFileParser> configParse;
    std::list<std::string> configFileInfo;
    ConfigFileParser::readingDataFromFile(configFileInfo, configFilePath);
    ConfigFileParser::startParsingFile(configFileInfo, configParse);
    ConfigFileParser::printingParsingData(configParse);
}

*/

int main ( int ac, char **av, char **env )
{
	(void)env;
	if (ac > 2)
        errorPrinting("too many arguments.");
	
    std::string configFilePath;
	
	configFilePath = (ac == 2) ? std::string(av[1]) : DEFAULT_CONFIG_FILE_NAME;
	ConfigFileParser		ConfigFile(configFilePath);
 
	// MultiHttpServer			MultiServers; 
	// std::list<ClientInfo>	ClientInfoList;
	ConfigFile.parseConfigFile();
	// std::cout << "HERE EXIST" << std::endl;
	MultiHttpServer MultiServers(ConfigFile.listOfServersConfiguration);
	MultiServers.setUpServers();
	MultiServers.startServers();
	return (1);
}
