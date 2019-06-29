#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFF_SIZE 3
#define N 4

struct buff 
{
    char array[BUFF_SIZE]; 
    int in;
    int out;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
};

void buff_init(struct buff *buff)
{
    buff->in = 0;
    buff->out = 0;
    pthread_mutex_init(&buff->mutex, NULL);
    pthread_cond_init(&buff->cond, NULL);
}

int buff_get(struct buff *buff)
{
    int item;
    item = buff->array[buff->out];
    buff->out = (buff->out + 1) % BUFF_SIZE;
    return item;
}
void buff_put(struct buff *buff, int item)
{
    buff->array[buff->in] = item;
    buff->in = (buff->in + 1) % BUFF_SIZE;
}
int buff_is_empty(struct buff *buff)
{
    return buff->in == buff->out;
}
int buff_is_full(struct buff *buff)
{
    return (buff->in + 1) % BUFF_SIZE == buff->out;
}

struct param {
    struct buff *prev;
    struct buff *next;
};

void *thread_entry(void *arg)
{
    struct param *param = (struct param *)arg;
    int i, item;
    for(i = 0; i < N; i++)
    {
        pthread_mutex_lock(&param->prev->mutex);
        while(buff_is_empty(param->prev))
            pthread_cond_wait(&param->prev->cond, &param->prev->mutex);
        item = buff_get(param->prev);
        printf("Thread ID: %lu  \nget %d\n", pthread_self(), item);
        item++;
        pthread_cond_signal(&param->next->cond);
        pthread_mutex_unlock(&param->prev->mutex);

        pthread_mutex_lock(&param->next->mutex);
        while(buff_is_full(param->next))
            pthread_cond_wait(&param->next->cond, &param->next->mutex);
        buff_put(param->next, item);
        printf("    put %d\n", item);
        pthread_cond_signal(&param->prev->cond);
        pthread_mutex_unlock(&param->next->mutex);
    }
    return NULL;
}

int main()
{
    struct param arg[N];
    int i;
    for(i = 0; i < N; i++)
    {
        arg[i].prev = (struct buff *)malloc(sizeof(struct buff));
        buff_init(arg[i].prev);
    }
    for(i = 0; i < N - 1; i++)
    {
        arg[i].next = arg[i+1].prev;
    }
    arg[N-1].next = arg[0].prev; // 形成缓冲区环
    buff_put(arg[0].prev, 0);
    pthread_t pid[N];
    for(i = 0; i < N; i++)
    {
        pthread_create(&pid[i], NULL, (void *)thread_entry, (void *)&arg[i]);
    }
    for(i = 0; i < N; i++)
    {
        pthread_join(pid[i], NULL);
    }
    return 0;
}