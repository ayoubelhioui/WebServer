#include <dirent.h>
#include <stdlib.h>
#include <cstdio>
int main(){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("/Users/ijmari/Desktop/")) != NULL) {
  	/* print all the files and directories within directory */
  	while ((ent = readdir (dir)) != NULL) {
    	printf ("dir : %s\n", ent->d_name);
 	 }
 	 closedir (dir);
	} else {
  /* could not open directory */
 	 perror ("");
	  return EXIT_FAILURE;
	}
}
