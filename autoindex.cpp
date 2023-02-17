#include <iostream>
#include <dirent.h>

using namespace std;

int main() {
    cout << "Content-type: text/html\n\n";
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

