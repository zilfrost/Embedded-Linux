#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t tid1, tid2;

static void *thr_handler(void *args){
    if(pthread_equal(pthread_self(), tid1) != 0){
        printf("Thread with ID <%lu> is running!\n", pthread_self());
        pthread_exit(NULL);

    } else{
        printf("Thread with ID <%lu> is running!\n", pthread_self());
        pthread_exit(NULL);
    }
}

int main(int argc, char const *argv[]) {
    int ret;

    if(ret = pthread_create(&tid1, NULL, &thr_handler, NULL)){
        printf("pthread_create() error number = %d\n", ret);
        return -1;
    }

    if(ret = pthread_create(&tid2, NULL, &thr_handler, NULL)){
        printf("pthread_create() error number = %d\n", ret);
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("End of main thread\n");
}
