#include "../Interfaces/GETMethod.hpp"
#include "../errorsHandling/errorsHandling.hpp"
# include "../webserver.hpp"

std::string GETMethod::get_file_type(mode_t mode) {
    if (S_ISREG(mode)) {
        return "File";
    } else if (S_ISDIR(mode)) {
        return "Directory";
    } else if (S_ISLNK(mode)) {
        return "Symbolic link";
    } else if (S_ISFIFO(mode)) {
        return "FIFO/pipe";
    } else if (S_ISBLK(mode)) {
        return "Block device";
    } else if (S_ISCHR(mode)) {
        return "Character device";
    } else if (S_ISSOCK(mode)) {
        return "Socket";
    } else {
        return "Unknown";
    }
}

std::string GETMethod::format_date(time_t t) {
    struct tm* tm = localtime(&t);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}

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
		if(loc.isDirectoryListingOn && !is_file_last){
            std::string root = loc.Root;
            if(root[root.length() - 1] != '/') root += '/';
            if(root[0] != '.') root = '.' + root;
            if(full_path[0] != '.') full_path = '.' + full_path;
            directoryListing(root, full_path);
            return "directoryListing.html";
        }
        else{
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
		    		if(check_file){
                        // const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
                        // std::cout << "FINAL PATH " << final_path << std::endl;
						// std::cout << "cgi_format " << cgi_format << std::endl;
						// std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
                        // for( ; CGIit != loc.CGI.end(); CGIit++ ){
                        //     if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
                        //         // php cgi
                        //     }
                        //     else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
                        //         // python cgi 
                        //     }
                        // }
                        return final_path;
                    }
		    		else ;
		    	}
		    }
		    else{
		    	std::string final_path = root + file;
		    	if(final_path[0] == '/') final_path = '.' + final_path;
		    	std::ifstream check_file(final_path, std::ios::binary);
		    	if(check_file){
					// const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
					// std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
                    // for( ; CGIit != loc.CGI.end(); CGIit++ ){
                    //     if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
                    //         std::cout << "i'm inside php cgi" << std::endl;
					// 		exit(0);
                    //     }
                    //     else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
                    //         // python cgi 
                    //     }
                    // }
					return final_path;
				}
		    	else ;
		    }
        }
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

void    GETMethod::directoryListing(std::string rootDirectory, std::string linking_path){
	DIR* dir = opendir(rootDirectory.c_str());
    if (dir == NULL) {
        
    }
    std::string file_list = "<!DOCTYPE html>\n"
                             "<html>\n"
                             "<head><title>Index of /</title></head>\n"
                             "<body>\n"
                             "<h1>Index of /</h1>\n"
                             "<table>\n"
                             "<tr><th>Name</th><th>Size</th><th>Last modified</th><th>Type</th></tr>\n";

    struct dirent *entry;
    struct stat filestat;
    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s%s", rootDirectory.c_str(), entry->d_name);
        if(stat(path, &filestat) == -1){
            continue;
        }
        std::string filename = std::string(entry->d_name);
        std::string filesize = std::to_string(filestat.st_size);
        std::string filemodtime = format_date(filestat.st_mtime);
        std::string filetype = get_file_type(filestat.st_mode);

        file_list += "<tr><td><a href=\"" + linking_path + filename + "\">" + filename + "</a></td><td>" + filesize + "</td><td>" + filemodtime + "</td><td>" + filetype + "</td></tr>\n";
	}
	file_list += "</table>\n"
				"</body>\n"
				"</html>\n";
	closedir(dir);
    std::ofstream directoryListingFile("directoryListing.html");
    directoryListingFile << file_list;
    directoryListingFile.close();
}
