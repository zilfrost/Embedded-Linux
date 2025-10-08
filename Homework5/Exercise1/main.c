#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int count = 0;

void sigint_handler(int sig) {
    count++;
    printf("SIGINT received (%d)\n", count);
    if (count >= 3) {
        printf("Exiting program...\n");
        _exit(0);
    }
}

int main() {
    signal(SIGINT, sigint_handler);

    while (1) {
        printf("Running... Press Ctrl+C\n");
        sleep(2);
    }
    return 0;
}
