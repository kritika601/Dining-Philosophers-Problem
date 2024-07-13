#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t s1,s2,s3;
int capacity;
void load() {
    printf("Car Loading...\n");
    sleep(1);
}
void unload() {
    printf("Car Unloading..\n");
    sleep(1);
}
void help1(){
    for (int i = 0; i < capacity; ++i) {
            sem_post(&s2);
        }
}
void* car(void* args) {
    pthread_mutex_t ride = PTHREAD_MUTEX_INITIALIZER;
    int count=0;
    while (count<1000) {
        load();
        help1();
        sleep(2);
        unload();
        for (int k= 0; k < capacity; ++k) {
            sem_post(&s3);
        }
        pthread_mutex_lock(&ride);
        pthread_mutex_unlock(&ride);
    }
    return NULL;
}

void* passenger(void* args) {
    int id = *((int*)args);
    free(args);
    int count=0;
    while (count<1000) {
        sem_wait(&s2);
        printf("Passenger %d boards the car.\n", id+1);
        sem_wait(&s3);
        printf("Passenger %d unboards the car.\n", id+1);
    }
    return NULL;
}

void initialize_sem(){
    sem_init(&s1, 0, 1);
    sem_init(&s2, 0, 0);
    sem_init(&s3, 0, 0);
}
void destroy_sem(){
    sem_destroy(&s1);
    sem_destroy(&s2);
    sem_destroy(&s3);
}
int main() {
    int tot_pass;
    printf("Enter the total number of passengers: ");
    scanf("%d", &tot_pass);
    printf("Enter the car capacity: ");
    scanf("%d", &capacity);
    if (tot_pass <= 0||capacity <= 0 ) {
        fprintf(stderr, "Error: Invalid input.\n");
        exit(0);
    }
    initialize_sem();
    pthread_t car_thread;
    pthread_create(&car_thread, NULL, car, NULL);
    pthread_t passenger_threads[100];
    for (int i = 0; i < tot_pass; ++i) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&passenger_threads[i], NULL, passenger, id);
    }
    pthread_join(car_thread, NULL);
    for (int i = 0; i < tot_pass; ++i) {
        pthread_join(passenger_threads[i], NULL);
    }
    destroy_sem();
    return 0;
}
