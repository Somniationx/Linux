#include <stdio.h>
#include <unistd.h>
//#include <sys/types.h>

int main() {

    while(1) {
        printf("I'm a process! My pid:%d\n", getpid());
        sleep(1);
    }
    return 0;
}
