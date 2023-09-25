#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t child_pid = fork();
    
    if (child_pid == -1) {
        // 错误处理
        perror("fork");
        return 1;
    }
    
    if (child_pid == 0) {
        // 子进程代码
        while (1) {
            printf("Child: My PID=%d, My parent's PID=%d\n", getpid(), getppid());
            sleep(3); // 可以添加适当的延迟，以控制打印速率
        }
    } else {
        // 父进程代码
        while (1) {
            printf("Parent: My PID=%d, My parent's PID=%d\n", getpid(), getppid());
            sleep(3); // 可以添加适当的延迟，以控制打印速率
        }
    }
    
    return 0;
}

