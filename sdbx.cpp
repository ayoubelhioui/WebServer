
# include "parsing/parsing.hpp"

void	server_start(std::list<Parsing> &servers)
{
	int								server_socket;
	std::list<Parsing>::iterator	it;
	int								maxSocket;
	int								received;
	int								prob;


	it = servers.begin();
	server_socket = create_socket(*it);
	std::list<client_info *> client_data;
	maxSocket = 0;
	received = 0;
	prob = 0;
	while (1)
	{
		fd_set	reads,writes;
		
	}
}