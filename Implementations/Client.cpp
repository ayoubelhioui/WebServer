#include "../Interfaces/Client.hpp"

	
client_info::client_info() : received(0), isFirstRead(true), bodyFirstRead(false), boundarySize(0), bytesToReceive(0), _maxSocket(0)
{
	 this->address_length = sizeof(this->address);
}

client_info::~client_info()
{
}

void	client_info::clients_Setup()
{
	client_info::_maxSocket = 0;
	FD_ZERO(&reads);
    FD_SET(server_socket, &reads);
    FD_ZERO(&writes);
    FD_SET(server_socket, &writes);
    _maxSocket = std::max(_maxSocket, server_socket);
    std::list<client_info *>::iterator client_data_it1 = client_data.begin();
    for (; client_data_it1 != client_data.end(); client_data_it1++)
    {
        FD_SET((*client_data_it1)->socket, &reads);
        FD_SET((*client_data_it1)->socket, &writes);
        std::max(_maxSocket, (*client_data_it1)->socket); // ????
    }
    if( select(_maxSocket + 1, &reads, &writes, NULL, NULL) == -1){
		errorPrinting("select has failed");
	}
}

void	client_info::checkingClientListenning(int server_socket, std::list<client_info *> &client_data
, fd_set &reads, fd_set &writes){
	if (FD_ISSET(server_socket, &reads))
    {
        client_info *client = client_info::get_client(-1, client_data);
        client->socket = accept(server_socket, (struct sockaddr *) &(client->address), &(client->address_length));
//           fcntl(client->socket, F_SETFL, O_NONBLOCK);
        FD_SET(client->socket, &reads);
        FD_SET(client->socket, &writes);
        _maxSocket = std::max(_maxSocket, client->socket);
        if (client->socket < 0) std::cerr << "accept function failed\n";
    }
}

client_info *client_info::get_client(int socket, std::list<client_info *> &data_list)
{
    std::list<client_info *> copy = data_list;
    std::list<client_info *>::iterator it = copy.begin();
    for(; it != copy.end(); it++)
        if(socket == (*it)->socket) return *it;
    client_info *new_node = new client_info;
    data_list.push_front(new_node);
    return new_node;
}
