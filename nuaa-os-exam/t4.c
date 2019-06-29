#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex1, mutex2;
pthread_cond_t signal1, signal2;
int t1 = 0, t2 = 0, t3 = 0;

void *T1_entry(void *arg)
{
	pthread_mutex_lock(&mutex1);
	sleep(2);
	puts("T1");
	t1 = 1;
	pthread_cond_signal(&signal1);
	pthread_mutex_unlock(&mutex1);
}
void *T2_entry(void *arg)
{
	pthread_mutex_lock(&mutex1);
	while(!t1)
		pthread_cond_wait(&signal1, &mutex1);
	pthread_mutex_unlock(&mutex1);
	sleep(2);
	puts("T2");
	pthread_mutex_lock(&mutex2);
	t2 = 1;
	pthread_cond_signal(&signal2);
	pthread_mutex_unlock(&mutex2);
}
void *T3_entry(void *arg)
{
	pthread_mutex_lock(&mutex1);
	while(!t1)
		pthread_cond_wait(&signal1, &mutex1);
	pthread_mutex_unlock(&mutex1);
	sleep(2);
	puts("T3");
	pthread_mutex_lock(&mutex2);
	t3 = 1;
	pthread_cond_signal(&signal2);
	pthread_mutex_unlock(&mutex2);
}
void *T4_entry(void *arg)
{
	pthread_mutex_lock(&mutex1);
	while(!t2 || !t3)
		pthread_cond_wait(&signal2, &mutex1);
	sleep(2);
	puts("T4");
	pthread_mutex_lock(&mutex1);
}
int main(int argc, char *argv[])
{
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_cond_init(&signal1, NULL);
	pthread_cond_init(&signal2, NULL);

	pthread_t tid[4];
	pthread_create(&tid[3], NULL, T4_entry, NULL);
	pthread_create(&tid[2], NULL, T3_entry, NULL);
	pthread_create(&tid[1], NULL, T2_entry, NULL);
	pthread_create(&tid[0], NULL, T1_entry, NULL);
	int i;
	for(i = 0; i < 4; i++)
	{
		pthread_join(tid[i], NULL);
	}
	return 0;
}