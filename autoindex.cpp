#include <set>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <utility>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <algorithm>
#include <fcntl.h>
#include <cstdio>
#include <dirent.h>

using namespace std;

int main() {
    char *buffer = new char[1024];
	sprintf(buffer, "HTTP/1.1 200 OK\r\n");
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Connection: close\r\n");
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Content-Length: %d\r\n", client->served_size);
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "Content-Type: %s\r\n", "");
	send(client->socket, buffer, strlen(buffer), 0);
	sprintf(buffer, "\r\n");
	send(client->socket, buffer, strlen(buffer), 0);
    cout << "<html><head><title>Directory Listing</title></head><body>";
    DIR *dir = opendir(".");
    if (dir == NULL) {
        cout << "<h1>Could not open directory</h1>";
    } else {
        cout << "<h1>Directory Listing</h1>";
        cout << "<ul>";

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            cout << "<li>" << entry->d_name << "</li>";
        }
        cout << "</ul>";
        closedir(dir);
    }
    cout << "</body></html>";
    return 0;
}

