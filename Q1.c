#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
pthread_mutex_t bowls[2];
pthread_mutex_t forks[5];
pthread_cond_t eat[5];

enum { THINKING, HUNGRY, EATING } state[5];
void thinking(int phil_id) {
    printf("Philosopher %d is thinking\n", phil_id+1);
    usleep(rand() % 1000000);
}
void eating(int phil_id) {
    printf("Philosopher %d is eating\n", phil_id+1);
    usleep(rand() % 1000000); 
}
void fork_dealing(int phil_id,int a){
    if (a==1){
        pthread_mutex_lock(&forks[phil_id]);
        state[phil_id] = HUNGRY;
        pthread_cond_signal(&eat[phil_id]);
        pthread_mutex_unlock(&forks[phil_id]);
    }
    else if (a==0){
        pthread_mutex_lock(&forks[phil_id]);
        state[phil_id] = THINKING;
        pthread_cond_signal(&eat[(phil_id + 1) % 5]);
        pthread_cond_signal(&eat[(phil_id + 4) % 5]);
        pthread_mutex_unlock(&forks[phil_id]);
    }
}
int check_deadlock() {
    int deadlock_count = 0;
    for (int i = 0; i < 5;++i) {
        if (state[i] == HUNGRY && state[(i + 1) % 5] == HUNGRY && state[(i + 4) % 5] == HUNGRY) {
            deadlock_count++;
        }
    }
    if (deadlock_count == 5) {
        printf("Deadlock encountered. Error Handled! \n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void* philosopher(void* args) {
    int phil_id = *((int*)args);
    int count=0;
    while (count<1000) {
        thinking(phil_id);
        fork_dealing(phil_id,1);
        pthread_mutex_lock(&forks[(phil_id + 1) % 5]);
        pthread_mutex_lock(&forks[(phil_id + 4) % 5]);
        eating(phil_id);
        pthread_mutex_unlock(&forks[(phil_id + 1) % 5]);
        pthread_mutex_unlock(&forks[(phil_id + 4) % 5]);
        fork_dealing(phil_id,0);
        usleep(rand() % 1000000);
        check_deadlock();
        count++;
    }
    return NULL;
}

void creation(pthread_t philosophers[], int ids[]){
    int count=0;
    while(count<5) {
        pthread_mutex_init(&forks[count+1], NULL);
        pthread_cond_init(&eat[count+1], NULL);
        state[count+1] = THINKING;
        count++;
    }
    pthread_mutex_init(&bowls[1], NULL);
    pthread_mutex_init(&bowls[2], NULL);
    for (int i = 0; i < 5; ++i) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }
    for (int i = 0; i < 5; ++i) {
        pthread_join(philosophers[i], NULL);
    }
    for (int i = 0; i < 5; ++i) {
        pthread_mutex_destroy(&forks[i]);
        pthread_cond_destroy(&eat[i]);
    }
    pthread_mutex_destroy(&bowls[1]);
    pthread_mutex_destroy(&bowls[2]);
}

int main() {
    pthread_t philosophers[5];
    int ids[5];
    creation(philosophers,ids);
    return 0;
}
