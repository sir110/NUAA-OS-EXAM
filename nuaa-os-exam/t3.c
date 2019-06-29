#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int *calc(void *i)
{
	int j, *sum;
	sum = (int*)malloc(sizeof(int));
	*sum = 0;
	for(j = 1; j <= 10; j++)
	{
		*sum += (*(int *)i * 10 + j);
	}
	return (void*)sum;
}

int main(int argc, char *argv[])
{
	pthread_t tid[10];
	int i, *result, sum = 0, p[10];
	result = (int *)malloc(sizeof(int));
	for(i = 0; i < 10; i++)
	{
		p[i] = i;
		pthread_create(&tid[i], NULL, (void*)calc, (void*)&p[i]);
	}
	for(i = 0; i < 10; i++)
	{
		pthread_join(tid[i], (void**)&result);
		sum += *result;
	}
	printf("%d\n", sum);
	return 0;
}