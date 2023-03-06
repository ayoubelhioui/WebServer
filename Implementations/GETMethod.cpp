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

void GETMethod::callGET(ClientInfo *client, ServerConfiguration &serverConfig) {
    client->checkPathValidation(client, serverConfig, client->parsedRequest.requestDataMap["path"]);
    std::cout << "GET PATH IS " << client->servedFileName << std::endl;
    if (client->servedFileName == "" && !client->inReadCgiOut) {
        throw std::runtime_error("file path not allowed");
    }
    if (client->inReadCgiOut == 0)
    {
    //std::cout << "in CGI CGI CGI" << std::endl;
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
    std::string newFile = "FilesForServing/" + client->generateRandString() + ".html";
    std::ofstream directoryListingFile(newFile);
    directoryListingFile << file_list;
    directoryListingFile.close();
    return newFile;
}