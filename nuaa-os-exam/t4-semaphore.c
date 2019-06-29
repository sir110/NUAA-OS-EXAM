#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

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
sema_t mutex;
sema_t wait1, wait2, wait3, wait4;

void *T1_entry(void *arg)
{
	sema_wait(&wait1);
	sema_wait(&mutex);
	sleep(2);
	puts("T1");
	sema_signal(&mutex);
	sema_signal(&wait2);
}
void *T2_entry(void *arg)
{
	sema_wait(&wait2);
	sema_wait(&mutex);
	sleep(2);
	puts("T2");
	sema_signal(&mutex);
	sema_signal(&wait2);
	sema_signal(&wait3);
}
void *T3_entry(void *arg)
{
	sema_wait(&wait2);
	sema_wait(&mutex);
	sleep(2);
	puts("T3");
	sema_signal(&mutex);
	sema_signal(&wait2);
	sema_signal(&wait4);
}
void *T4_entry(void *arg)
{
	sema_wait(&wait3);
	sema_wait(&wait4);
	sema_wait(&mutex);
	sleep(2);
	puts("T4");
	sema_signal(&mutex);
}
int main(int argc, char *argv[])
{
	sema_init(&mutex, 1);
	sema_init(&wait1, 1);
	sema_init(&wait2, 0);
	sema_init(&wait3, 0);
	sema_init(&wait4, 0);

	pthread_t tid[4];
	int i;
	pthread_create(&tid[0], NULL, T4_entry, NULL);
	pthread_create(&tid[1], NULL, T3_entry, NULL);
	pthread_create(&tid[2], NULL, T2_entry, NULL);
	pthread_create(&tid[3], NULL, T1_entry, NULL);
	for(i = 0; i < 4; i++)
	{
		pthread_join(tid[i], NULL);
	}
	return 0;
}