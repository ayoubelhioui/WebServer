#include "../Interfaces/Client.hpp"
	
ClientInfo::ClientInfo( void ) : isFirstRead(true) , addressLength(sizeof(this->address)), inReadCgiOut(0), stillWaiting(0), isFirstCgiRead(0)
{
}

ClientInfo::~ClientInfo( void )
{
}		


ClientInfo	&ClientInfo::operator= ( const ClientInfo &obj )
{
	(void)obj;
	// if (this == &obj)
	// 	return (*this);
	// return (*this);
	return (*this);
}

ClientInfo::ClientInfo ( const ClientInfo &obj )
{
	(void)obj;
	// std::cout << " Yes i got called\n";
	// *this = obj;
}



// void	ClientInfo::clients_Setup()
// {
// 	ClientInfo::_maxSocket = 0;
// 	FD_ZERO(&reads);
//     FD_SET(server_socket, &reads);
//     FD_ZERO(&writes);
//     FD_SET(server_socket, &writes);
//     _maxSocket = std::max(_maxSocket, server_socket);
//     std::list<ClientInfo *>::iterator client_data_it1 = client_data.begin();
//     for (; client_data_it1 != client_data.end(); client_data_it1++)
//     {
//         FD_SET((*client_data_it1)->socket, &reads);
//         FD_SET((*client_data_it1)->socket, &writes);
//         std::max(_maxSocket, (*client_data_it1)->socket); // ????
//     }
//     if( select(_maxSocket + 1, &reads, &writes, NULL, NULL) == -1){
// 		errorPrinting("select has failed");
// 	}
// }

// void	ClientInfo::checkingClientListenning(int server_socket, std::list<ClientInfo *> &client_data
// , fd_set &reads, fd_set &writes){
// 	if (FD_ISSET(server_socket, &reads))
//     {
//         ClientInfo *client = ClientInfo::get_client(-1, client_data);
//         client->socket = accept(server_socket, (struct sockaddr *) &(client->address), &(client->address_length));
// //           fcntl(client->socket, F_SETFL, O_NONBLOCK);
//         FD_SET(client->socket, &reads);
//         FD_SET(client->socket, &writes);
//         _maxSocket = std::max(_maxSocket, client->socket);
//         if (client->socket < 0) std::cerr << "accept function failed\n";
//     }
// }

// ClientInfo *ClientInfo::get_client(int socket, std::list<ClientInfo *> &data_list)
// {
//     std::list<ClientInfo *> copy = data_list;
//     std::list<ClientInfo *>::iterator it = copy.begin();
//     for(; it != copy.end(); it++)
//         if(socket == (*it)->socket) return *it;
//     ClientInfo *new_node = new ClientInfo;
//     data_list.push_front(new_node);
//     return new_node;
// }
