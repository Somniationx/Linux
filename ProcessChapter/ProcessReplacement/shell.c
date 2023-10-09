#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>

// ANSI颜色代码
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM 1024
#define OPT_NUM 64

char lineCommand[NUM];
char *myargv[OPT_NUM];
int lastCode = 0;
int lastSig = 0;

void print_prompt() {
    // 获取当前用户名
    char *username = getlogin();
    if (username == NULL) {
        perror("getlogin");
        exit(1);
    }

    // 获取主机名
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        exit(1);
    }

    // 获取当前路径
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(1);
    }

    char *basename_cwd = basename(cwd);

    // 格式化带颜色的提示符
    printf(ANSI_COLOR_RED "[%s" ANSI_COLOR_RESET "@" ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " %s]$ ", username,
           hostname, basename_cwd);
    fflush(stdout);
}

// 添加ls的颜色选项
void add_ls_color_option() {
    int i = 0;
    while (myargv[i] != NULL) {
        if (strcmp(myargv[i], "ls") == 0) {
            int j = 0;
            while (myargv[j] != NULL) {
                j++;
            }
            myargv[j] = (char *) "--color=auto";  // 添加ls的颜色选项
            myargv[j + 1] = NULL;
            break;
        }
        i++;
    }
}

int main() {
    while (1) {
        print_prompt();

        char *s = fgets(lineCommand, sizeof(lineCommand) - 1, stdin);
        if (s == NULL) {
            perror("fgets");
            exit(1);
        }
        lineCommand[strlen(lineCommand) - 1] = '\0';

        myargv[0] = strtok(lineCommand, " ");
        int i = 1;
        while (myargv[i++] = strtok(NULL, " "));

        if (myargv[0] != NULL) {
            if (strcmp(myargv[0], "cd") == 0) {
                if (myargv[1] != NULL) {
                    if (chdir(myargv[1]) != 0) {
                        perror("chdir");
                    }
                } else {
                    fprintf(stderr, "cd: missing argument\n");
                }
                continue;
            }

            if (strcmp(myargv[0], "exit") == 0) {
                exit(0);
            }

            if (myargv[0] != NULL && myargv[1] != NULL && strcmp(myargv[0], "echo") == 0) {
                if (strcmp(myargv[1], "$?") == 0) {
                    printf("%d, %d\n", lastCode, lastSig);
                } else {
                    printf("%s\n", myargv[1]);
                }
                continue;
            }
            pid_t id = fork();
            if (id == -1) {
                perror("fork");
                exit(1);
            }

            if (id == 0) {
                add_ls_color_option(); // 在ls命令中添加--color=auto选项
                execvp(myargv[0], myargv);
                perror("execvp");
                exit(1);
            }

            int status = 0;
            pid_t ret = waitpid(id, &status, 0);
            if (ret < 0) {
                perror("waitpid");
                exit(1);
            }
            lastCode = WEXITSTATUS(status);
            lastSig = WTERMSIG(status);
        }
    }
}
