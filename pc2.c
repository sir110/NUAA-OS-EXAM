#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int buffer1[4], buffer2[4];
int in, out;
int comp2, cons;
int buffer1_is_empty()
{
    return in == out;
}
int buffer1_is_full()
{
    return (in + 1) % 4 == out;
}
int buffer2_is_empty()
{
    return comp2 == cons;
}
int buffer2_is_full()
{
    return (cons + 1) % 4 == comp2;
}
int get_item1()
{
    int item;
    item = buffer1[out];
    out = (out + 1) % 4;
    return item;
}
int get_item2()
{
    int item;
    item = buffer2[cons];
    cons = (cons + 1) % 4;
    return item;
}
void put_item1(int item)
{
    buffer1[in] = item;
    in = (in + 1) % 4;
}
void put_item2(int item)
{
    buffer2[comp2] = item;
    comp2 = (comp2 + 1) % 4;
}

typedef struct{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}sema_t;
void sema_init(sema_t *sema, int value)
{
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}
void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    while(sema->value <= 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}
void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}
sema_t mutex1;
sema_t empty1;
sema_t full1;
sema_t mutex2;
sema_t empty2;
sema_t full2;

void produce()
{
    int i, item;
    for(i=0; i<8; i++)
    {
        sema_wait(&empty1);
        sema_wait(&mutex1);
        item = 'a' + i;
        put_item1(item);
        printf("produce item: %c\n", item);
        sema_signal(&mutex1);
        sema_signal(&full1);
    }
}
void calculate()
{
    int i, item;
    for(i=0; i<8; i++)
    {
        sema_wait(&full1);
        sema_wait(&mutex1);
        item = get_item1();
        item -= 32;
        sema_signal(&mutex1);
        sema_signal(&empty1);

        sema_wait(&empty2);
        sema_wait(&mutex2);
        put_item2(item);
        sema_signal(&mutex2);
        sema_signal(&full2);
    }
}
void consume()
{
    int i, item;
    for(i=0; i<8; i++)
    {
        sema_wait(&full2);
        sema_wait(&mutex2);
        item = get_item2();
        printf("    consume item: %c\n", item);
        sema_signal(&mutex2);
        sema_signal(&empty2);
    }
}

int main()
{
    pthread_t tid[2];
    sema_init(&mutex1, 1);
    sema_init(&mutex2, 1);
    sema_init(&empty1, 3);
    sema_init(&full1, 0);
    sema_init(&empty2, 3);
    sema_init(&full2, 0);
    pthread_create(&tid[0], NULL, (void *)&consume, NULL);
    pthread_create(&tid[1], NULL, (void *)&calculate, NULL);
    produce();
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    return 0;
}

