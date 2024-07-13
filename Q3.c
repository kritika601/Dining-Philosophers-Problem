#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
sem_t mutex, bridge, l_mutex, r_mutex;
int left_cars, right_cars;
void passing(int direction) {
    if (direction == 1) {
        printf("Car from left is passing the bridge\n");
    } 
    else {
        printf("Car from right is passing the bridge\n");
    }
}
void locking(sem_t a,sem_t b){
    sem_post(&a);
    sem_wait(&b);
}
void unlocking(sem_t a,sem_t b){
    sem_post(&b);
    sem_wait(&a);
}
void* left(void* args) {
    int car_id = *((int*)args);
    sem_wait(&mutex);
    if (left_cars < 5) {
        left_cars++;
        passing(1);
        sem_post(&mutex);
        sem_wait(&bridge);
        sleep(1); 
        sem_post(&bridge);
        sem_wait(&mutex);
        left_cars--;
    } else {
        sem_post(&mutex);
        int wait_time = 0;
        while (wait_time < 5) {
            sem_wait(&l_mutex);
            if (left_cars < 5) {
                left_cars++;
                passing(1);
                sem_post(&l_mutex);
                sem_wait(&bridge);
                sleep(1);
                sem_post(&bridge);
                sem_wait(&l_mutex);
                left_cars--;
                sem_post(&l_mutex);
                break;
            }
            sem_post(&l_mutex);
            sleep(1);
            wait_time++;
        }
    }
    sem_post(&mutex);
    pthread_exit(NULL);
}

void* right(void* args) {
    int car_id = *((int*)args);
    sem_wait(&mutex);
    if (right_cars < 5) {
        right_cars++;
        passing(2);
        sem_post(&mutex);
        sem_wait(&bridge);
        sleep(1); 
        sem_post(&bridge);
        sem_wait(&mutex);
        right_cars--;
    } else {
        sem_post(&mutex);
        int wait_time = 0;
        while (wait_time < 5) {
            sem_wait(&r_mutex);
            if (right_cars < 5) {
                right_cars++;
                passing(2);
                sem_post(&r_mutex);
                sem_wait(&bridge);
                sleep(1); 
                sem_post(&bridge);
                sem_wait(&r_mutex);
                right_cars--;
                sem_post(&r_mutex);
                break;
            }
            sem_post(&r_mutex);
            sleep(1);
            wait_time++;
        }
    }
    sem_post(&mutex);
    pthread_exit(NULL);
}
void mutex_initiation(){
    sem_init(&mutex, 0, 1);
    sem_init(&bridge, 0, 5);
    sem_init(&l_mutex, 0, 1);
    sem_init(&r_mutex, 0, 1);
}
void mutex_destruction(){
    sem_destroy(&mutex);
    sem_destroy(&bridge);
    sem_destroy(&l_mutex);
    sem_destroy(&r_mutex);
}
int main() {
    pthread_t threads[100];
    int lc, rc;
    printf("Enter the number of cars on the left: ");
    scanf("%d", &lc);
    printf("Enter the number of cars on the right: ");
    scanf("%d", &rc);
    mutex_initiation();
    int i;
    for (i = 0; i < lc; ++i) {
        pthread_create(&threads[i], NULL, left, &i);
    }
    for (int j = 0; j < rc; ++j) {
        pthread_create(&threads[i + j], NULL, right, &j);
    }
    for (i = 0; i < lc + rc; ++i) {
        pthread_join(threads[i], NULL);
    }
    mutex_destruction();
    return 0;
}
