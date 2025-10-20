#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>

#define QUEUE_NAME "/my_task_queue"
#define MAX_SIZE 1024

mqd_t mq; // descriptor cho message queue

void cleanup(int sig) {
    printf("\nReceived SIGINT — closing queue...\n");
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    printf("Message queue closed and unlinked.\n");
    exit(0);
}

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;       // tối đa 10 tin nhắn
    attr.mq_msgsize = MAX_SIZE; // kích thước tối đa mỗi tin
    attr.mq_curmsgs = 0;

    signal(SIGINT, cleanup);

    // Tạo hoặc mở hàng đợi (read only)
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    printf("Worker started. Waiting for tasks...\n");

    char buffer[MAX_SIZE + 1];
    unsigned int priority;

    while (1) {
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, &priority);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            printf("Processing task (Priority: %u): %s\n", priority, buffer);
            sleep(1);
        } else {
            perror("mq_receive");
        }
    }

    return 0;
}
