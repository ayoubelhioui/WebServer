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

bool    isSlash(char a){
    return (a == '/');

}

void    locationSplit(std::string &currentPath, std::string &pathOffset){
   std::string  splittedOffset;
    size_t foundSlash = currentPath.rfind('/');
    if(foundSlash)
    {
        splittedOffset = currentPath.substr(foundSlash);
        currentPath = currentPath.substr(0, foundSlash);
    }
    else
    {
        splittedOffset = currentPath;
        currentPath = "/";

    }
    pathOffset = splittedOffset + pathOffset;
}

bool    isThereFileLast(std::string &path,
                        bool &is_file_last,
                        int &index_last)
{
    bool point = 0;
    for(int len = index_last; len >= 0; len--) {
        if (path[len] == '.')
            point = 1;
        if (path[len] == '/' && point) {
            //std::cout << "LOCATION FOUND\n";
            is_file_last = 1;
            index_last = len;
            break;
        }
        else if (path[len] == '/' && !point){
            //std::cout << "LOCATION FILE NOT FOUND\n";
            return 0;
        }
    }
    if(!point){
        //std::cout << "LOCATION FILE NOT FOUND\n";
        return 0;
    }
    //std::cout << "LOCATION FOUND\n";
    return 1;
}

void    GETMethod::handleGETMethod(ClientInfo *client, ServerConfiguration &serverConfig) {
    std::string currentPath = client->parsedRequest.requestDataMap["path"], pathOffset = "";
    if (currentPath.back() == '/') {
        currentPath.pop_back();
    }
    if (currentPath.front() != '/') {
        currentPath = '/' + currentPath;
    }
    int SlashCounter = std::count_if(currentPath.begin(), currentPath.end(), isSlash) + 1;
    for (int i = 0; i < SlashCounter; i++) // looping through every splitted part of the path
    {
        for (std::list<LocationBlockParse>::iterator beg = serverConfig.Locations.begin();
             beg != serverConfig.Locations.end(); beg++)
        {
            LocationBlockParse currentLocation = *beg;
            std::string insideLocationPath = currentLocation.Location;
            if (insideLocationPath.back() == '/') {
                insideLocationPath.pop_back();
            }
            if (insideLocationPath.front() != '/') {
                insideLocationPath = '/' + insideLocationPath;
            }
            if (insideLocationPath != currentPath) {
                continue;
            }
            client->cgiIterator = (*beg).CGI.end();
            bool is_file_last = 0;
            int len = currentPath.length() - 1;
            int index_last = len;
            if (isThereFileLast(currentPath, is_file_last, index_last)) // checking the case where the path is already a file and the location exists
            {
                if (currentPath.front() != '.')
                    currentPath = '.' + currentPath;
                std::ifstream fileCheck(currentPath);
                if (fileCheck)
                { // checking if the file exists and capable to be read
                    client->_currentLocation = beg;
                    const char *cgi_format = strrchr(currentPath.c_str(), '.') + 1;
                    std::cout << "1cgi_format is " << cgi_format << std::endl;
                    std::list<std::pair<std::string, std::string> >::iterator CGIit = currentLocation.CGI.begin();
                    for (; CGIit != currentLocation.CGI.end(); CGIit++) { // looping through all the existing CGIS in the current found location
                        if (!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")) {
                            client->servedFileName = currentPath;
                            client->cgiIterator = CGIit;
                            std::cout << "HERE ! 1" << std::endl;
                            return;
                        } else if (!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")) {
                            // python cgi
                        }
                    }
                    client->servedFileName = currentPath;
                }
                return ;
            }
            else { // the case where the path is directory and must be joined with the offset (file or dir)
                
                if (currentLocation.Root.back() == '/' && currentLocation.Root.length() > 1)
                    currentLocation.Root.pop_back();
                currentPath = currentLocation.Root + pathOffset;
                int rootLength = currentPath.length() - 1;
                //std::cout << "the wanted case is " << currentPath << std::endl;
                if (isThereFileLast(currentPath, is_file_last, rootLength))
                {
                    std::ifstream fileCheck(currentPath);
                    if (fileCheck) {
                        client->_currentLocation = beg;
                        std::cout << "is equal : " << (client->cgiIterator != client->_currentLocation->CGI.end()) << std::endl;
                        const char *cgi_format = strrchr(currentPath.c_str(), '.') + 1;
                        std::cout << "2cgi_format is " << cgi_format << std::endl;
                        std::list<std::pair<std::string, std::string> >::iterator CGIit = currentLocation.CGI.begin();
                        for (; CGIit != currentLocation.CGI.end(); CGIit++) {
                            if (!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")) {
                                client->servedFileName = currentPath;
                                client->cgiIterator = CGIit;
                                return;
                            } else if (!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")) {
                                // python cgi
                                return;
                            }
                        }
                        client->servedFileName = currentPath;
                    }
//                    std::cout << "is equal : " << (client->cgiIterator != client->_currentLocation->CGI.end()) << std::endl;
                    std::cout << "222" << std::endl;
                    return ;
                }
                else // handle the case where the path + offset is a directory (must loop through indexes and check if there is a valid path)
                {
                    if(currentLocation.isDirectoryListingOn && client->parsedRequest.requestDataMap["method"] == "GET")
                        client->servedFileName = directoryListing(currentLocation.Root, currentPath, client);
                    else
                    {
                        for (std::list<std::string>::iterator index_it = currentLocation.indexFiles.begin();
                             index_it != currentLocation.indexFiles.end(); index_it++)
                        {
                            std::string final_path = currentPath + '/' + (*index_it);
                            std::ifstream check_file(final_path, std::ios::binary);
                            if (check_file)
                            {
                                client->_currentLocation = beg;
                                //std::cout << "I;M the winnnnnnnnnerrrr" << std::endl;
                                const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
                                std::cout << "3cgi_format is " << cgi_format << std::endl;
                                std::list<std::pair<std::string, std::string> >::iterator CGIit = currentLocation.CGI.begin();
                                for (; CGIit != currentLocation.CGI.end(); CGIit++) {
                                    if (!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")) {
                                        client->servedFileName = final_path;
                                        client->cgiIterator = CGIit;
                                        std::cout << "HERE ! 3" << std::endl;
                                        return;
                                    } else if (!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")) {
                                        // python cgi
                                    }
                                }
                                client->servedFileName = final_path;
                            }
                        }
                    }
                    return ;
                }

            }
        }
        locationSplit(currentPath, pathOffset);
        //	std::string path = client->parsedRequest.requestDataMap["path"];
//	for (std::list<LocationBlockParse>::iterator beg = serverConfig.Locations.begin(); beg != serverConfig.Locations.end(); beg++){
//		LocationBlockParse loc = *beg;
//		std::string res = loc.Location;
//		int len = path.length() - 1;
//		int	index_last = len;
//		if(path[len] == '/')
//			len--;
//		bool is_file_last = 0, point = 0;
//		for(; len >= 0; len--){
//			if(path[len] == '.')
//				point = 1;
//			if(path[len] == '/' && point){
//				is_file_last = 1;
//				index_last = len;
//				break;
//			}
//			else if (path[len] == '/' && !point) break;
//		}
//		if(res[res.length() - 1] != '/') res += '/';
//		std::string full_path = path.substr(0, index_last + 1);
//		if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
//		if(full_path != res) continue;
//		if(loc.isDirectoryListingOn && !is_file_last){
//            std::string root = loc.Root;
//            if(root[root.length() - 1] != '/') root += '/';
//            if(root[0] != '.') root = '.' + root;
//            if(full_path[0] != '.') full_path = '.' + full_path;
//            client->servedFileName = directoryListing(root, full_path, client);
//            return ;
//        }
//        else{
//		    std::string file = path.substr(index_last + 1);
//		    if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
//		    if(is_file_last && file[file.length() - 1] == '/') file.erase(file.length() - 1);
//		    std::string root = loc.Root;
//		    if(root[root.length() - 1] != '/') root += '/';
//		    if(file == ""){
//		    	for(std::list<std::string>::iterator index_it = loc.indexFiles.begin(); index_it != loc.indexFiles.end(); index_it++)
//		    	{
//		    		std::string final_path = root + (*index_it);
//		    		if(final_path[0] == '/') final_path = '.' + final_path;
//		    		std::ifstream check_file(final_path, std::ios::binary);
//		    		if(check_file){
//					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
//					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
//                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
//                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
//                            client->cgiContentLength =  "0";
//                            client->cgiContentType = "";
//                            client->CGIexecutedFile(final_path, client, serverConfig, CGIit);
//                            return ;
//                        }
//                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
//                            // python cgi
//                        }
//                    }
//                    client->servedFileName = final_path;
//					return ;
//				}
//		    	else ;
//		    	}
//		    }
//		    else{
//		    	std::string final_path = root + file;
//		    	if(final_path[0] == '/') final_path = '.' + final_path;
//		    	std::ifstream check_file(final_path, std::ios::binary);
//		    	if(check_file){
//					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
//					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
//                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
//                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
//                            client->CGIexecutedFile(final_path, client, serverConfig, CGIit);
//                            return ;
//                        }
//                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
//                            // python cgi
//                        }
//                    }
//                    client->servedFileName = final_path;
//					return ;
//				}
//		    	else ;
//		    }
//        }
//	}
    }
}

    void GETMethod::callGET(ClientInfo *client, ServerConfiguration &serverConfig) {
        handleGETMethod(client, serverConfig);
        if (client->servedFileName == "" && !client->inReadCgiOut) {
            throw std::runtime_error("file path not allowed");
        }
        if (client->inReadCgiOut == 0) {
//    //std::cout << "in CGI CGI CGI" << std::endl;
            client->served.open(client->servedFileName, std::ios::binary);
            client->served.seekg(0, std::ios::end);
            client->served_size = client->served.tellg();
            client->served.seekg(0, std::ios::beg);
            std::string buffer = "HTTP/1.1 200 OK\r\n"
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
                                            ClientInfo *client) {
        DIR *dir = opendir(rootDirectory.c_str());
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
            if (stat(path, &filestat) == -1) {
                continue;
            }
            std::string filename = std::string(entry->d_name);
            std::string filesize = std::to_string(filestat.st_size);
            std::string filemodtime = format_date(filestat.st_mtime);
            std::string filetype = get_file_type(filestat.st_mode);

            file_list +=
                    "<tr><td><a href=\"" + linking_path + filename + "\">" + filename + "</a></td><td>" + filesize +
                    "</td><td>" + filemodtime + "</td><td>" + filetype + "</td></tr>\n";
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