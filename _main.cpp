

// # include "webserver.hpp"
# include "Interfaces/ConfigFileParser.hpp"


int main ( void )
{
	ConfigFileParser		ConfigFile;
	// MultiHttpServer			MultiServers;
	// std::list<ClientInfo>	ClientInfoList;



	ConfigFile.parseConfigFile();

	
	// MultiServers = MultiHttpServer(ConfigFile.listOfServersConfiguration);
	// MultiServers.setUpServers();
	// MultiServers.startServers();

	
}
