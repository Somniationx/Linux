#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <executable_path>\n", argv[0]);
        return 1;
    }

    const char *bin = argv[1]; // Get the executable path from the command line

    pid_t id = fork(); // Create a child process
    if (id == -1) {
        perror("fork"); // Print an error message if fork fails
        return 1;
    }

    if (id == 0) { // Child process
        sleep(3); // Child process sleeps for 3 seconds
        printf("Child process is running...\n");

        // Execute the specified external executable
        execl(bin, bin, NULL);

        // If execl fails, report the error and exit
        perror("execl");
        exit(1);
    } else { // Parent process
        int status = 0;
        printf("Father process is running...\n");

        // Wait for the child process to complete and obtain its exit status
        pid_t ret = waitpid(id, &status, 0);
        if (ret == -1) {
            perror("waitpid"); // Print an error message if waitpid fails
            return 1;
        }

        // Check if the child process exited normally
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            printf("Child process exited with status: %d\n", exit_status);
        }
        // Check if the child process was terminated by a signal
        else if (WIFSIGNALED(status)) {
            int signal_num = WTERMSIG(status);
            printf("Child process terminated by signal: %d\n", signal_num);
        }
        // If none of the above, the child process terminated abnormally
        else {
            printf("Child process terminated abnormally\n");
        }

        printf("Father process running done...\n");
    }

    return 0; // Exit the parent process
}

