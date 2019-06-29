#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define N 1000000
#define N_CPU_CORE 8

struct param 
{
    int start;
};
struct result 
{
    double sum;
};

void *compute(void *arg) 
{
    struct param *param;
    struct result *result;
    double sum = 0;
    int i;
    param = (struct param*) arg;
    for (i = param->start; i <= N; i += 16)
    {
        sum += pow(-1, (i-1) / 2) * 1/i;
    }
    result = (struct result *)malloc(sizeof(struct result));
    result->sum = sum;
    return result;
}


int main (int argc, char *argv[]) 
{
    pthread_t tid[N_CPU_CORE];
    struct param params[N_CPU_CORE];
    struct param *param;
    struct result *result;
    int i;
    double pi = 0;
    for (i = 0; i < N_CPU_CORE; i++) 
    {
        // struct param *param;
        // 1  3  5  7    9  11  13  15 
        // 17 19
        param = &params[i];
        param->start = 2 * i + 1;
        pthread_create(&tid[i], NULL, compute, param);
    }
    for (i = 0; i < N_CPU_CORE; i++) 
    {
        // struct result *result;
        pthread_join(tid[i], (void **)&result);
        pi += result->sum;
        free(result);
    }
    printf("PI = %lf\n", pi * 4);
    return 0;
}