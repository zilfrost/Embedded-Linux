#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int shared_data = 0;

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *reader_func(void *arg) {

    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);
        printf("[Reader %lu] đọc shared_data = %d\n", (unsigned long)pthread_self(), shared_data);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return NULL;
}

void *writer_func(void *arg) {

    for (int i = 0; i < 5; i++) {
        pthread_rwlock_wrlock(&rwlock);
        shared_data++;
        printf("[Writer %lu] ghi shared_data -> %d\n", (unsigned long)pthread_self(), shared_data);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
    return NULL;
}

int main(void) {
    pthread_t readers[5], writers[2];

    for (long i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader_func, NULL);
    }

    for (long i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer_func, NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}
