#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    int ret_val;
    int status;

    ret_val = fork();

    if(ret_val == 0) {
        while(1) {
            printf("PID: %d, PPID: %d\n", getpid(), getppid());
            sleep(1);
        }
        exit(0);
    } else if(ret_val > 0){
        printf("\nThis is the parent process");
    }
    return 0;
}