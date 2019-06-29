#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define N 1000000
double sum1 = 0, sum2 = 0;

void *worker(void *arg) 
{
    int i;    
    for (i = N + 1; i <= 2 * N - 1; i += 2)
        sum1 += pow(-1 , (i-1) / 2) * 1/i;
}

void master() 
{
    int i;
    for (i = 1; i <= N - 1; i += 2 ) 
        sum2 += pow(-1 , (i-1) / 2) * 1/i;
}

int main (int argc, char *argv[]) 
{
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL);
    master();
    pthread_join(tid, NULL);
    double pi = (sum1 + sum2) * 4;
    printf("PI = %lf\n", pi);
    return 0;
}