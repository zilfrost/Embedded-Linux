#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define QUEUE_NAME "/my_task_queue"
#define MAX_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <priority> \"<task_description>\"\n", argv[0]);
        exit(1);
    }

    unsigned int priority = atoi(argv[1]);
    const char *task = argv[2];

    mqd_t mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_send(mq, task, strlen(task), priority) == -1) {
        perror("mq_send");
        exit(1);
    }

    printf("Task sent (Priority: %u): %s\n", priority, task);
    mq_close(mq);

    return 0;
}
