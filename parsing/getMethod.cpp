#include "getMethod.hpp"

std::string	GetMethod::handleGetMethod(std::map<std::string, std::string> &request, Parsing &server){
	std::string path = request["path"];
	for (std::list<locationBlock>::iterator beg = server.Locations.begin(); beg != server.Locations.end(); beg++){
		locationBlock loc = *beg;
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
	}
	return "";
}

void	GetMethod::callGet(client_info *client){
	std::string path = handle_get_method(client->request_data, *it);
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
	sprintf(buffer, "\r\n");
	send(client->socket, buffer, strlen(buffer), 0);
}

void	GetMethod::directoryListing(void){
	DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("/Users/ijmari/Desktop/")) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
    	printf ("dir : %s\n", ent->d_name);
    }
    closedir (dir);
    } else {
     perror ("");
      return EXIT_FAILURE;
    }
}