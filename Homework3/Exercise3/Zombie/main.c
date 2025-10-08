#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    int ret_val;
    int status;

    ret_val = fork();

    if(ret_val == 0) {
        printf("\nThis is a child process");
        exit(0);
    } else if(ret_val > 0){
        printf("\nThis is the parent process");
        sleep(30);
        printf("\nEnd of parent process");
    }

    return 0;
}