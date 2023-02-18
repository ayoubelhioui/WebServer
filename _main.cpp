

// # include "webserver.hpp"
# include "Interfaces/ConfigFileParser.hpp"

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
	ConfigFileParser		ConfigFile;
	if (ac > 2)
        errorPrinting("too many arguments.");
	
    std::string configFilePath = (ac == 2) ? std::string(av[1]) : DEFAULT_CONFIG_FILE_NAME;
	 std::list<std::string> configFileInfo;
 
	// MultiHttpServer			MultiServers;
	// std::list<ClientInfo>	ClientInfoList;



	ConfigFile.parseConfigFile();

	
	// MultiServers = MultiHttpServer(ConfigFile.listOfServersConfiguration);
	// MultiServers.setUpServers();
	// MultiServers.startServers();

	
}
