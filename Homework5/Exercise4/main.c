#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>

void sigint_handler(int sig) {
    printf("\nSIGINT received.\n");
}

void sigterm_handler(int sig) {
    printf("\nSIGTERM received. Exiting...\n");
    exit(0);
}

int main() {
    char buffer[256];
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL);
        if (ret > 0) {
            if (FD_ISSET(STDIN_FILENO, &fds)) {
                if (fgets(buffer, sizeof(buffer), stdin)) {
                    printf("You entered: %s", buffer);
                }
            }
        }
    }
    return 0;
}
