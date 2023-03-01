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

void    GETMethod::handleGETMethod(ClientInfo *client, ServerConfiguration &serverConfig){
	std::string path = client->parsedRequest.requestDataMap["path"];
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
            client->servedFileName = directoryListing(root, full_path, client);
            return ;
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
					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
                            client->CGIexecutedFile(final_path, client, serverConfig, CGIit);
                            return ;
                        }
                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
                            // python cgi 
                        }
                    }
                    client->servedFileName = final_path;
					return ;
				}
		    	else ;
		    	}
		    }
		    else{
		    	std::string final_path = root + file;
		    	if(final_path[0] == '/') final_path = '.' + final_path;
		    	std::ifstream check_file(final_path, std::ios::binary);
		    	if(check_file){
					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
                            client->CGIexecutedFile(final_path, client, serverConfig, CGIit);
                            return ;
                        }
                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
                            // python cgi 
                        }
                    }
                    client->servedFileName = final_path;
					return ;
				}
		    	else ;
		    }
        }
	}
}

void    GETMethod::callGET( ClientInfo *client, ServerConfiguration &serverConfig)
{
	handleGETMethod(client, serverConfig);
    if(client->servedFileName == "" && !client->inReadCgiOut){
		throw std::runtime_error("file path not allowed");
	}
    if(client->inReadCgiOut == 0){
//    std::cout << "in CGI CGI CGI" << std::endl;
	    client->served.open(client->servedFileName, std::ios::binary);
	    client->served.seekg(0, std::ios::end);
	    client->served_size = client->served.tellg();
	    client->served.seekg(0, std::ios::beg);
        std::string  buffer = "HTTP/1.1 200 OK\r\n"
								 + std::string("Connection: close\r\n")
								 + std::string("Content-Length: ")
								 + std::to_string(client->served_size)
								 + "\r\n"
								 + "Content-Type: "
								 + get_mime_format(client->servedFileName.c_str())
								 + "\r\n\r\n";
                                send(client->socket, buffer.c_str(), buffer.length(), 0);
    }
}

std::string GETMethod::directoryListing(std::string rootDirectory, std::string linking_path,
ClientInfo *client){
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
    std::string newFile = "FilesForServingGET/" + client->generateRandString() + ".html";
    std::ofstream directoryListingFile(newFile);
    directoryListingFile << file_list;
    directoryListingFile.close();
    return newFile;
}


