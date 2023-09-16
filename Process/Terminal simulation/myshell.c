#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>
#include <limits.h> // For PATH_MAX

#define NUM 1024
#define OPT_NUM 64

char lineCommand[NUM];
char *myargv[OPT_NUM]; // Pointer array

void executeCommand() {
    pid_t id = fork();
    assert(id != -1);

    if (id == 0) { // Child process
        if (execvp(myargv[0], myargv) == -1) {
            perror("execvp");
        }
        exit(1);
    } else { // Parent process
        waitpid(id, NULL, 0); // Wait for the child process to complete
    }
}

int main() {
    while (1) {
        char hostname[NUM];
        gethostname(hostname, sizeof(hostname));

        char cwd[PATH_MAX]; // Use PATH_MAX for maximum path length
        getcwd(cwd, sizeof(cwd));

        char username[NUM];
        strcpy(username, getenv("USER"));

        // Find the last occurrence of '/' in the path
        char *lastSlash = strrchr(cwd, '/');
        if (lastSlash != NULL) {
            strcpy(cwd, lastSlash + 1); // Copy the part after the last '/'
        }

        // Print a custom Linux-style command prompt with colored text
        printf("\033[1;32m[%s@%s \033[1;34m%s\033[0;32m]#\033[0m ", username, hostname, cwd);
        fflush(stdout);

        // Get user input
        if (fgets(lineCommand, sizeof(lineCommand), stdin) == NULL) {
            perror("fgets");
            return 1;
        }

        // Remove trailing newline
        lineCommand[strcspn(lineCommand, "\n")] = '\0';

        // Tokenize the input and store it in myargv
        char *token = strtok(lineCommand, " ");
        int argc = 0;
        while (token != NULL && argc < OPT_NUM - 1) {
            myargv[argc++] = token;
            token = strtok(NULL, " ");
        }
        myargv[argc] = NULL; // Null-terminate the array

        if (argc > 0) {
            executeCommand();
        }
    }

    return 0;
}

