#include "../Interfaces/GETMethod.hpp"
#include "../errorsHandling/errorsHandling.hpp"

std::string	GETMethod::handleGETMethod(std::map<std::string, std::string> &request, ServerConfiguration &serverConfig){
	std::string path = request["path"];
	for (std::list<LocationBlockParse>::iterator beg = serverConfig.Locations.begin(); beg != serverConfig.Locations.end(); beg++){
		LocationBlockParse loc = *beg;
		std::string res = loc.Location;
		int len = path.length() - 1; 
		int	index_last = len;
		if(path[len] == '/')
			len--;
		bool is_file_last = 0, point = 0;
		for(; len >= 0; len--){
			if(path[len] == '.')
				point = 1;
			if(path[len] == '/' && point){
				is_file_last = 1;
				index_last = len;
				break;
			}
			else if (path[len] == '/' && !point) break;
		}
		if(res[res.length() - 1] != '/') res += '/';
		std::string full_path = path.substr(0, index_last + 1);
		if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
		if(full_path != res) continue;
		// if(loc.Redirection.length() != 0){
			
		// }
		// else
		// {
		std::string file = path.substr(index_last + 1);
		if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
		if(is_file_last && file[file.length() - 1] == '/') file.erase(file.length() - 1);
		std::string root = loc.Root;
		if(root[root.length() - 1] != '/') root += '/';
		if(file == ""){
			for(std::list<std::string>::iterator index_it = loc.indexFiles.begin(); index_it != loc.indexFiles.end(); index_it++)
			{
				std::string final_path = root + (*index_it);
				if(final_path[0] == '/') final_path = '.' + final_path;
				std::ifstream check_file(final_path, std::ios::binary);
				if(check_file){return final_path;}
				else ;
			}
		}
		else{
			std::string final_path = root + file;
			if(final_path[0] == '/') final_path = '.' + final_path;
			std::ifstream check_file(final_path, std::ios::binary);
			if(check_file){return final_path;}
			else ;
		}	
		// }
	}
	return "";
}

bool	GETMethod::callGET( ClientInfo *client, ServerConfiguration &serverConfig, std::list<ClientInfo *>::iterator &ClientInfoIt )
{
	std::string path = handleGETMethod(client->parsedRequest.requestDataMap, serverConfig);
	if(path == ""){
		error_404(ClientInfoIt);
		return 1;
	}
	client->served.open(path, std::ios::binary);
	client->served.seekg(0, std::ios::end);
	client->served_size = client->served.tellg();
	client->served.seekg(0, std::ios::beg);
	char *buffer = new char[1024];
	sprintf(buffer, "HTTP/1.1 200 OK\r\n");
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Connection: close\r\n");
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Content-Length: %d\r\n", client->served_size);
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Content-Type: %s\r\n", get_mime_format(path.c_str()));
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "\r\n"); // * 
	send(client->socket, buffer, strlen(buffer), 0);
	return 0;
}

int	GETMethod::directoryListing(char *rootDirectory, SOCKET socket){
	(void)rootDirectory;
	(void)socket;
	// char *buffer = new char[1024]();
	// sprintf(buffer, "Content-type: text/html\r\n");
    // send(socket, buffer, sizeof(buffer), 0);
	// sprintf(buffer, "<html><head><title>Directory Listing</title></head><body>\r\n");
	// send(socket, buffer, sizeof(buffer), 0);
    // DIR *dir = opendir(rootDirectory);
    // if (dir == NULL) {
	// 	sprintf(buffer, "<h1>Could not open directory</h1>\r\n");
	// 	send(socket, buffer, sizeof(buffer), 0);
    // } else {
	// 	sprintf(buffer, "<h1>Directory Listing</h1>\r\n");
	// 	send(socket, buffer, sizeof(buffer), 0);
    //     sprintf(buffer, "<ul>\r\n");
	// 	send(socket, buffer, sizeof(buffer), 0);

    //     struct dirent *entry;
    //     while ((entry = readdir(dir)) != NULL) {
    //         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
    //             continue;
    //         }
    //         sprintf(buffer, "<li> %s <li>\r\n", entry->d_name);
	// 		send(socket, buffer, sizeof(buffer), 0);
    //     }
    //     sprintf(buffer, "<ul>\r\n");
	// 	send(socket, buffer, sizeof(buffer), 0);
    //     closedir(dir);
    // }
	// sprintf(buffer, "</body></html>\r\n");
	// send(socket, buffer, sizeof(buffer), 0);
	// delete [] buffer;
	return (1);
}