#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char *argv[]) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {

        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        char *args[] = {"/bin/ls", "-l", argv[1], NULL};
        execve("/bin/ls", args, NULL);
        perror("execve");
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);
        char buffer[BUFSIZ];
        ssize_t nread;
        while ((nread = read(pipefd[0], buffer, BUFSIZ)) > 0) {
            std::cout << "\nI READ " << nread << std::endl;
            write(STDOUT_FILENO, buffer, nread);
        }
        if (nread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        waitpid(pid, NULL, 0);
	}
}

