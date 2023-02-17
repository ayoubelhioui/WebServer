#ifndef PARSING_REQUEST
#define PARSING_REQUEST
	#include <map>
	#include <string>
	#include <sstream>
	#include <sys/socket.h>
	#define MAX_REQUEST_SIZE 2000

	class ParsingRequest{
		public :
			int	receivedBytes;
			int bytesToReceive;
			char *requestHeader;
			int contentLength;
			int bodyIndex;
			std::map<std::string, std::string> request_data;
			void	parse();
			void	parsingRequestFirstLine(std::string);
			void	parsingRequest(std::string);
			void	receiveFirstTime ( SOCKET );
			void	parsingMiniHeader( ); 

			int		retIndex(char *);
			ParsingRequest();
			~ParsingRequest();
	};
#endif

/*

void receiveFromClient(client_info *client, int &received)
{
    client->bytesToReceive = (client->received + MAX_REQUEST_SIZE < client->contentLength) ? MAX_REQUEST_SIZE : client->contentLength - client->received;
    received = recv(client->socket, client->requestHeader, client->bytesToReceive, 0);
    std::cout << "i have received : " << received << " and the content length : " << client->contentLength << std::endl;
    client->received += received;
    client->requestHeader[received] = 0;
}


void gettingNewBodyIndex(client_info *client, std::string &boundarySavior)
{
    client->bodyIndex += ret_index(client->requestHeader + client->bodyIndex) + 4;
    client->boundarySize = boundarySavior.length() - boundarySavior.find("=") + 3;
}

void gettingFileName(client_info *client, std::string &newString)
{
    int newContentIndex = newString.find("filename=");
    int DoubleQuotePosition = newString.find("\"", newContentIndex + 11);
    client->uploadFileName = newString.substr(newContentIndex + 10, DoubleQuotePosition - newContentIndex - 10);
}

void parsingMiniHeader(client_info *client)
{
    std::map<std::string, std::string>::iterator content = client->request_data.find("Content-Type:");
    std::string boundarySavior = content->second;
    std::string newString(client->requestHeader + client->bodyIndex);
    gettingFileName(client, newString);
    gettingNewBodyIndex(client, boundarySavior);
}
*/