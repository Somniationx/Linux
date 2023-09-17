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
char *myargv[OPT_NUM]; // Pointer array to store command arguments
int lastCode; // To store the exit code of the last executed command
int lastSig;  // To store the signal information of the last executed command

// Function to execute a command
void executeCommand() {
    pid_t id = fork();
    assert(id != -1);

    if (id == 0) { // Child process
        if (execvp(myargv[0], myargv) == -1) {
            perror("execvp");
            exit(1);
        }
    } else { // Parent process
        int status = 0;
        pid_t ret = waitpid(id, &status, 0); // Wait for the child process to complete
        assert(ret > 0);
        (void)ret;
        lastCode = (status>>8) & 0xFF;
        lastSig = status & 0x7F;
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
        if (token != NULL && strcmp(token, "ls") == 0) {
            myargv[argc++] = token;
            myargv[argc++] = (char*)"--color=auto";
            token = strtok(NULL, " ");
        }
        while (token != NULL && argc < OPT_NUM - 1) {
            myargv[argc++] = token;
            token = strtok(NULL, " ");
        }
        myargv[argc] = NULL; // Null-terminate the array

        if (argc > 0) {
            if (strcmp(myargv[0], "ll") == 0) {
                myargv[0] = (char*)"ls";
                myargv[1] = (char*)"--color=auto";
                myargv[2] = (char*)"-l";
                myargv[3] = NULL;
            } else if (strcmp(myargv[0], "cd") == 0) {
                if (myargv[1] != NULL) {
                    if (chdir(myargv[1]) != 0) {
                        perror("chdir");
                    }
                    continue;
                }
            } else if (myargv[0] != NULL && myargv[1] != NULL && strcmp(myargv[0], "echo") == 0) {
                int i = 1;
                while (myargv[i] != NULL) {
                    if (strcmp(myargv[i], "$?") == 0) {
                        printf("%d, %d\n", lastCode, lastSig);
                    } else {
                        printf("%s ", myargv[i]);
                    }
                    i++;
                }
                printf("\n");
                continue;
            }
            executeCommand();
        }
    }

    return 0;
}
