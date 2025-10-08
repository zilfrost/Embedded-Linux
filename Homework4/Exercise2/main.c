#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3
#define INCREMENT 1000000

long long counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void* increase_counter(){
    for(int i = 0; i < INCREMENT; i++){
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

int main(){
    pthread_t threads[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++){
        if(pthread_create(&threads[i], NULL, increase_counter, NULL)){
            printf("Error creating thread\n");
            return 1;
        }
    }

    for(int i = 0; i < NUM_THREADS; i++){
        if(pthread_join(threads[i], NULL)){
            perror("Error joining thread\n");
            return 1;
        }
    }

    printf("Final counter value: %lld\n", counter);
    pthread_mutex_destroy(&counter_mutex);
    return 0;
}
