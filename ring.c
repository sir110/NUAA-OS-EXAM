#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 10
pthread_mutex_t mutex;
pthread_cond_t cond;

void *fun(void *arg)
{
    pthread_mutex_lock(&mutex);
    int *p = (int*)arg;
    printf("Thread:T%d   values:%d\n", (*p) + 1, (*p) + 1);
    (*p)++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return (void*)p;
}

int main()
{
    pthread_t tid[N];
    int *result = (int*)malloc(sizeof(int));
    *result = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    int i = 0, count = 12;
    while(count--)
    {
        pthread_create(&tid[i], NULL, fun, (void*)result);
        pthread_join(tid[i], (void **)&result);
        i = (i + 1) % N;
    }
    return 0;
}