#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

int data = 0;
int data_ready = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void* producer(void* args){
    for(int i = 0; i < 10; i++){
        pthread_mutex_lock(&lock);
        data = rand() % 100;
        data_ready = 1;
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    return NULL;
}

void *consumer(void* args){
    while(1){
        while(!data_ready){
            pthread_cond_wait(&condition, &lock);
        }
        printf("Consumed data: %d\n", data);
        data_ready = 0;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(){
    pthread_t prod_thread, cons_thread;

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condition);

    return 0;
}
