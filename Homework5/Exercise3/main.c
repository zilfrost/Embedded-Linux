#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void child_handler(int sig) {
    printf("Received signal from parent\n");
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {  
        // Process con
        signal(SIGUSR1, child_handler);
        while (1) {
            pause(); // chờ tín hiệu từ cha
        }
    } else {         
        // Process cha
        for (int i = 0; i < 5; i++) {
            sleep(2);
            kill(pid, SIGUSR1);
        }
        printf("Parent done. Exiting...\n");
        kill(pid, SIGTERM); // kết thúc con
        wait(NULL);
    }
    return 0;
}
