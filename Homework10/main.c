// File: restaurant_ipc.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>

#define BUFFER_SIZE 10
#define VEG 1
#define NONVEG 2

typedef struct {
    int vegan_tray[BUFFER_SIZE];
    int nonvegan_tray[BUFFER_SIZE];

    sem_t vegan_mutex;
    sem_t vegan_empty;
    sem_t vegan_full;

    sem_t nonvegan_mutex;
    sem_t nonvegan_empty;
    sem_t nonvegan_full;
} SharedData;

void random_sleep(int min, int max) {
    int t = rand() % (max - min + 1) + min;
    sleep(t);
}

void print_tray_status(SharedData *data) {
    int veg_count = 0, nonveg_count = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (data->vegan_tray[i] != 0) veg_count++;
        if (data->nonvegan_tray[i] != 0) nonveg_count++;
    }
    printf("[MAIN] Items in vegan tray: %d/%d | non-vegan tray: %d/%d\n",
           veg_count, BUFFER_SIZE, nonveg_count, BUFFER_SIZE);
}

void chef_donatello(SharedData *data) {
    srand(getpid());
    char *menu[] = {"Fettuccine Chicken Alfredo", "Garlic Sirloin Steak"};
    while (1) {
        sem_wait(&data->nonvegan_empty);
        sem_wait(&data->nonvegan_mutex);

        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (data->nonvegan_tray[i] == 0) {
                data->nonvegan_tray[i] = NONVEG;
                printf("[Chef Donatello] Added %s at slot %d\n",
                       menu[rand() % 2], i);
                break;
            }
        }

        sem_post(&data->nonvegan_mutex);
        sem_post(&data->nonvegan_full);
        random_sleep(1, 5);
    }
}

void chef_portecelli(SharedData *data) {
    srand(getpid());
    char *menu[] = {"Pistachio Pesto Pasta", "Avocado Fruit Salad"};
    while (1) {
        sem_wait(&data->vegan_empty);
        sem_wait(&data->vegan_mutex);

        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (data->vegan_tray[i] == 0) {
                data->vegan_tray[i] = VEG;
                printf("[Chef Portecelli] Added %s at slot %d\n",
                       menu[rand() % 2], i);
                break;
            }
        }

        sem_post(&data->vegan_mutex);
        sem_post(&data->vegan_full);
        random_sleep(1, 5);
    }
}

void customer_vegan(SharedData *data, int id) {
    srand(getpid());
    while (1) {
        sem_wait(&data->vegan_full);
        sem_wait(&data->vegan_mutex);

        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (data->vegan_tray[i] != 0) {
                data->vegan_tray[i] = 0;
                printf("[Customer %d - Vegan] Took a vegan dish from slot %d\n", id, i);
                break;
            }
        }

        sem_post(&data->vegan_mutex);
        sem_post(&data->vegan_empty);
        random_sleep(10, 15);
    }
}

void customer_nonvegan(SharedData *data, int id) {
    srand(getpid());
    while (1) {
        sem_wait(&data->nonvegan_full);
        sem_wait(&data->nonvegan_mutex);

        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (data->nonvegan_tray[i] != 0) {
                data->nonvegan_tray[i] = 0;
                printf("[Customer %d - NonVegan] Took a non-vegan dish from slot %d\n", id, i);
                break;
            }
        }

        sem_post(&data->nonvegan_mutex);
        sem_post(&data->nonvegan_empty);
        random_sleep(10, 15);
    }
}

void customer_hybrid(SharedData *data, int id) {
    srand(getpid());
    while (1) {
        // Lấy món chay
        sem_wait(&data->vegan_full);
        sem_wait(&data->vegan_mutex);
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (data->vegan_tray[i] != 0) {
                data->vegan_tray[i] = 0;
                printf("[Customer %d - Hybrid] Took a vegan dish from slot %d\n", id, i);
                break;
            }
        }
        sem_post(&data->vegan_mutex);
        sem_post(&data->vegan_empty);

        // Lấy món không chay
        sem_wait(&data->nonvegan_full);
        sem_wait(&data->nonvegan_mutex);
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (data->nonvegan_tray[i] != 0) {
                data->nonvegan_tray[i] = 0;
                printf("[Customer %d - Hybrid] Took a non-vegan dish from slot %d\n", id, i);
                break;
            }
        }
        sem_post(&data->nonvegan_mutex);
        sem_post(&data->nonvegan_empty);

        random_sleep(10, 15);
    }
}

int main() {
    SharedData *data = mmap(NULL, sizeof(SharedData),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    for (int i = 0; i < BUFFER_SIZE; i++) {
        data->vegan_tray[i] = 0;
        data->nonvegan_tray[i] = 0;
    }

    sem_init(&data->vegan_mutex, 1, 1);
    sem_init(&data->vegan_empty, 1, BUFFER_SIZE);
    sem_init(&data->vegan_full, 1, 0);

    sem_init(&data->nonvegan_mutex, 1, 1);
    sem_init(&data->nonvegan_empty, 1, BUFFER_SIZE);
    sem_init(&data->nonvegan_full, 1, 0);

    pid_t pid;
    if ((pid = fork()) == 0) chef_donatello(data);
    if ((pid = fork()) == 0) chef_portecelli(data);
    if ((pid = fork()) == 0) customer_nonvegan(data, 1);
    if ((pid = fork()) == 0) customer_vegan(data, 2);
    if ((pid = fork()) == 0) customer_hybrid(data, 3);

    // Tiến trình cha
    while (1) {
        print_tray_status(data);
        sleep(10);
    }

    munmap(data, sizeof(SharedData));
    return 0;
}
