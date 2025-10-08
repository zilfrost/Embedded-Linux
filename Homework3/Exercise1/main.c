#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    int ret_val;
    int status;
    int child_pid, parent_pid;

    ret_val = fork();

    if(ret_val == 0) {
        // Child process
        child_pid = getpid();
        printf("\nIm the child process");
        printf("\nMy PID: %d", child_pid);
        exit(10); //exit with status 10
    } else if(ret_val > 0){
        // Parent process
        parent_pid = getpid();
        printf("\nIm the parent process");
        printf("\nMy PID: %d, My child PID: %d\n", parent_pid, child_pid);
        wait(&status);
        if (WIFEXITED(status)) {
            printf("\nChild exited normally with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("\nChild did not exit normally\n");
        }
    } else {
        printf("Error!\n");
    }

    return 0;
}