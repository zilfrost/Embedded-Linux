#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
    int ret_val;
    int status;
    int child_pid, parent_pid;

    setenv("MY_COMMAND", "ls", 1);

    ret_val = fork();

    if(ret_val == 0) {
        // Child process
        char *cmd = getenv("MY_COMMAND");
        execlp(cmd, cmd, (char *)NULL);
    } else if(ret_val > 0){
        wait(NULL);
    } else {
        printf("Error!\n");
    }

    return 0;
}