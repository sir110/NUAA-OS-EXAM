#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int buffer1[4], buffer2[4];
int in1=0, out1=0;
int in2=0, out2=0;
int buffer1_is_empty()
{
    return in1 == out1;
}
int buffer1_is_full()
{
    return (in1 + 1) % 4 == out1;
}
int buffer2_is_empty()
{
    return in2 == out2;
}
int buffer2_is_full()
{
    return (in2 + 1) % 4 == out2;
}
int get_item1()
{
    int item;
    item = buffer1[out1];
    out1 = (out1 + 1) % 4;
    return item;
}
int get_item2()
{
    int item;
    item = buffer2[out2];
    out2 = (out2 + 1) % 4;
    return item;
}
void put_item1(int item)
{
    buffer1[in1] = item;
    in1 = (in1 + 1) % 4;
}
void put_item2(int item)
{
    buffer2[in2] = item;
    in2 = (in2 + 1) % 4;
}

pthread_mutex_t mutex1, mutex2;
pthread_cond_t empty1, full1, empty2, full2;

void produce()
{
    int i;
    int item;

    for(i=0; i<8; i++)
    {
        pthread_mutex_lock(&mutex1);
        while(buffer1_is_full())
            pthread_cond_wait(&empty1, &mutex1);

        item = 'a' + i;
        put_item1(item);
        printf("produce item: %c\n", item);

        pthread_cond_signal(&full1);
        pthread_mutex_unlock(&mutex1);
    }
}

void consume()
{
    int i;
    int item;

    for(i=0; i<8; i++)
    {
        pthread_mutex_lock(&mutex2);
        while(buffer2_is_empty())
            pthread_cond_wait(&full2, &mutex2);

        item = get_item2();
        printf("    consume item: %c\n", item);
        pthread_cond_signal(&empty2);
        pthread_mutex_unlock(&mutex2);
    }
}
void calculate()
{
    int i, item;
    for(i=0; i<8; i++)
    {
        pthread_mutex_lock(&mutex1);
        while(buffer1_is_empty())
            pthread_cond_wait(&full1, &mutex1);
        item = get_item1();
        item -= 32;
        pthread_cond_signal(&empty1);
        pthread_mutex_unlock(&mutex1);

        pthread_mutex_lock(&mutex2);
        while(buffer2_is_full())
            pthread_cond_wait(&empty2, &mutex2);
        put_item2(item);
        pthread_cond_signal(&full2);
        pthread_mutex_unlock(&mutex2);
    }
}

int main()
{
    pthread_t tid[2];
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&empty1, NULL);
    pthread_cond_init(&full1, NULL);
    pthread_cond_init(&empty2, NULL);
    pthread_cond_init(&full2, NULL);
    pthread_create(&tid[0], NULL, (void *)&consume, NULL);
    pthread_create(&tid[1], NULL, (void *)&calculate, NULL);
    produce();
    int i;
    for(i=0; i<2; i++)
        pthread_join(tid[i], NULL);
    return 0;
}

