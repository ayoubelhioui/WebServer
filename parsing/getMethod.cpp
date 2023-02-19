#include "../Interfaces/getMethod.hpp"
std::string get_file_type(mode_t mode) {
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

std::string format_date(time_t t) {
    struct tm* tm = localtime(&t);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}

std::string	GETMethod::handleGETMethod(std::map<std::string, std::string> &request, Parsing &server){
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
		if(loc.Redirection.length() != 0){
			
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

void	GETMethod::callGET(client_info *client){
	std::string path = handle_get_method(client->parsedRequest.request_data, *it);
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

int	GETMethod::directoryListing(char *rootDirectory, int socket){
	DIR* dir = opendir(rootDirectory);
    if (dir == NULL) {
        perror("Error opening directory");
        return 1;
    }
    std::string file_list = "<!DOCTYPE html>\n"
                             "<html>\n"
                             "<head><title>Index of /</title></head>\n"
                             "<body>\n"
                             "<h1>Index of /</h1>\n"
                             "<table>\n"
                             "<tr><th>Name</th><th>Size</th><th>Last modified</th><th>Type</th></tr>\n";

    struct dirent* entry;
    struct stat filestat;
    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &filestat) < 0) {
            perror("Error getting file information");
            continue;
        }

        std::string filename = std::string(entry->d_name);
        std::string filesize = std::to_string(filestat.st_size);
        std::string filemodtime = format_date(filestat.st_mtime);
        std::string filetype = get_file_type(filestat.st_mode);

        file_list += "<tr><td><a href=\"" + filename + "\">" + filename + "</a></td><td>" + filesize + "</td><td>" + filemodtime + "</td><td>" + filetype + "</td></tr>\n";
		file_list += "</table>\n"
					"</body>\n"
					"</html>\n";
	}
	closedir(dir);

	const char* data = file_list.c_str();
	int data_len = strlen(data);

	if (send(sockfd, data, data_len, 0) != data_len) {
	    perror("Error sending directory listing");
	    return 1;
	}

	close(sockfd);

}
/*
#include <iostream>
#include <cstring>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <ctime>
#include <iomanip>

std::string get_file_type(mode_t mode) {
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

std::string format_date(time_t t) {
    struct tm* tm = localtime(&t);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(1234);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    DIR* dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        return 1;
    }

    std::string file_list = "<!DOCTYPE html>\n"
                             "<html>\n"
                             "<head><title>Index of /</title></head>\n"
                             "<body>\n"
                             "<h1>Index of /</h1>\n"
                             "<table>\n"
                             "<tr><th>Name</th><th>Size</th><th>Last modified</th><th>Type</th></tr>\n";

    struct dirent* entry;
    struct stat filestat;
    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &filestat) < 0) {
            perror("Error getting file information");
            continue;
        }

        std::string filename = std::string(entry->d_name);
        std::string filesize = std::to_string(filestat.st_size);
        std::string filemodtime = format_date(filestat.st_mtime);
        std::string filetype = get_file_type(filestat.st_mode);

        file_list += "<tr><td><a href=\"" + filename + "\">" + filename + "</a></td><td>" + filesize + "</td><td>" + filemodtime + "</td><td>"

*/