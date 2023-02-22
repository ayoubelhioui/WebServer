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

std::string	GETMethod::handleGETMethod(ParsingRequest &parsedData, ServerConfiguration &serverConfig){
	std::string path = parsedData.requestDataMap["path"];
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
            return directoryListing(root, full_path);
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
					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
                            return CGIexecutedFile(final_path, parsedData.queryString, serverConfig);
                        }
                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
                            // python cgi 
                        }
                    }
					return final_path;
				}
		    	else ;
		    }
        }
	}
	return "";
}

std::string GETMethod::callGET( ClientInfo *client, ServerConfiguration &serverConfig, std::list<ClientInfo *>::iterator &ClientInfoIt )
{
	std::string path = handleGETMethod(client->parsedRequest, serverConfig);
	if(path == ""){
		error_404(ClientInfoIt);
		return "";
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
	return path;
}

std::string    generateRandString ( int n )
{
    std::string         randString;

    const std::string    chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    
    for (int i = 0; i < n; ++i) {
        randString += chars[rand() % chars.size()];
    }
    return randString;
}

std::string GETMethod::directoryListing(std::string rootDirectory, std::string linking_path){
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
    std::string newFile = "FilesForServingGET/" + generateRandString(10) + ".html";
    std::ofstream directoryListingFile(newFile);
    directoryListingFile << file_list;
    directoryListingFile.close();
    return newFile;
}

std::string		GETMethod::CGIexecutedFile( std::string php_file, std::string queryString, ServerConfiguration &server ){
    int     pid = 0;
    const char * request_method = "GET";
    const char * script_name = "CGIS/php-cgi";
	const char * query_string = queryString.c_str();
    const char * server_name = server.serverHost.c_str();
    const char * server_port = server.serverPort.c_str();

    setenv("REQUEST_METHOD", request_method, 1);
    setenv("QUERY_STRING", query_string, 1);
    setenv("SCRIPT_NAME", script_name, 1);
    setenv("SERVER_NAME", server_name, 1);
    setenv("SERVER_PORT", server_port, 1);
    setenv("REDIRECT_STATUS", "200", 1);
    int fd[2];
    pipe(fd);
    std::string newFile = "FilesForServingGET/" + generateRandString(10) + ".html";
    std::ofstream out_file(newFile);
    pid = fork();
    if (pid == 0){
        char  *args[3];
        close(fd[0]);
        dup2(fd[1], 1);
        args[0] = (char *) script_name;
        args[1] = (char *) php_file.c_str();
        args[2] = NULL;
        if (execve(script_name, args, NULL) == -1)
            return "";
    }
    else if (pid > 0){
        close(fd[1]);
        char buffer[2001];
        ssize_t n;
        n = read(fd[0], buffer, 1000);
        buffer[n] = 0;
        out_file << bodyFromCgiHeader(buffer);
        while(n > 0){
            n = read(fd[0], buffer, 1000);
            buffer[n] = 0;
            out_file << buffer;
        }
        out_file.close();
        waitpid(pid, NULL, 0);
    }
    return newFile;
}
int     cgiretIndex(char *requestHeader){
    for(int i = 0; requestHeader[i]; i++){
      if(!strncmp(&requestHeader[i], "\r\n\r\n", 4))
          return i;
    }
    return -1;
}
std::string GETMethod::bodyFromCgiHeader(char *buffer){
    std::string stringBuffer(buffer);
    int body = cgiretIndex(buffer) + 4;
    stringBuffer = stringBuffer.substr(body);
    return stringBuffer;
}