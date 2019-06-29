### 题目1

1. 主进程创建1个子进程

2. 主进程通过管道与子进程连接

   * 子进程的标准输出连接到管道的写端

   - 主进程的标准输入连接到管道的读端

3. 在子进程中调用exec(“echo”, “echo”, “hello world”, NULL)

4. 在父进程中调用read(0, buf, sizeof(buf))，从标准输入中获取子进程发送的字符串，并打印出来

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int fd[2];
	char buf[32];
	pipe(fd);
	pid_t pid;
	pid = fork();
	if(pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], 1);
		execlp("echo", "echo", "hello world", NULL);
	}
	else if(pid > 0)
	{
		wait(NULL);
		close(fd[1]);
		dup2(fd[0], 0);
		int size = read(0, buf, sizeof(buf));
		//printf("%s", buf);
		write(1, buf, size);
	}
}
```



### 题目2

1. 主进程创建2个子进程，主进程通过两个管道分别与两个子进程连接
2. 第一个子进程计算从1加到50的和，并将结果通过管道送给父进程
3. 第一个子进程计算从50加到100的和，并将结果通过管道送给父进程
4. 父进程读取两个子进程的结果，将他们相加，打印出来，结果为5050

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int fd[2], fd2[2];
	int i, sum, p1, p2;
	pipe(fd); // 先创建管道，再创建子进程，子进程将继承文件描述符
	pipe(fd2);// 类似
	pid_t pid[2];
	pid[0] = fork();
	if(pid[0] > 0)
	{
		pid[1] = fork();
	}
	if(pid[0] == 0)
	{
		close(fd[0]);
		dup2(fd[1], 1);
		for(i = 1; i <= 50; i++)
			sum += i;
		write(1, &sum, sizeof(int));
		exit(0);
	}
	if(pid[1] == 0)
	{
		close(fd2[0]);
		dup2(fd2[1], 1);
		for(i = 51; i <= 100; i++)
			sum += i;
		write(1, &sum, sizeof(int));
		exit(0);
	}
	else
	{
		wait(NULL);
		close(fd[1]);
		dup2(fd[0], 0);
		read(0, &p1, sizeof(int));
		close(fd2[1]);
		dup2(fd2[0], 0);
		read(0, &p2, sizeof(int));
		printf("%d\n", p1 + p2);
	}
}
```



### 题目3

1、主线程创建10个子线程

第0个子线程计算从01加到10的和

第1个子线程计算从11加到20的和

第2个子线程计算从21加到30的和 

...

第9个子线程计算从91加到100的和

2、主线程归并10个子线程的计算结果，最终结果为5050 

3、本题必须使用线程参数来完成

```c
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
```

### 题目4

1、主线程创建4个子线程T1、T2、T3、T4，主线程在4个子线程退出后，才退出

2、线程T1、T2、T3、T4的运行时代码如下：

```c
#include <unistd.h> // sleep函数声明在该头文件中

void *T1_entry(void *arg)
{
    sleep(2);  // 睡眠2秒，不准删除此条语句，否则答题无效
    puts(“T1”);
}

void *T2_entry(void *arg)
{
    sleep(1);  // 睡眠1秒，不准删除此条语句，否则答题无效
    puts(“T2”);
}

void *T3_entry(void *arg)
{
    sleep(1);  // 睡眠1秒，不准删除此条语句，否则答题无效
    puts(“T3”);
}

void *T4_entry(void *arg)
{
    puts(“T4”);
}
```

3、使用信号量或者条件变量机制(而不是使用sleep函数)，使得这四个线程满足如下制约关系：

- T1的print语句执行后，T2和T3才可以执行print语句
- T2和T3的print语句执行后，T4才可以执行print语句

4、程序输出结果为

```
T1
T2
T3
T4
```

或者

```
T1
T3
T2
T4
```

```c
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
```



### 题目1：pipe.c

已经给了基本代码，在基本代码的基础上修改

1、使用fork、pipe实现cat /etc/passwd |grep root | wc -l

2、要求

* 本题不需要进行字符串处理，如将"cat /etc/passwd"分割为两个单词

* 可以在程序直接使用分割好的字符串数组["cat", "/etc/passwd"]

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 4
struct cmd {
    int argc;
    char *argv[MAX_ARGS];
};

struct cmd cmds[] = {
    {2, {"cat", "/etc/passwd", NULL}},
    {2, {"grep", "root", NULL}},
    {2, {"wc", "-l", NULL}},
};

void exec_pipe(int cmdc)
{
	int fd[2];
	int i;
	pid_t pid = fork();
    if (pid == 0) 
    {
		for(i = 0; i < cmdc - 1; i++)
		{
			pipe(fd); // 管道
	        int pipepid = fork();
	        if(pipepid == 0) 
	        {
	            dup2(fd[1], 1); 
	            close(fd[0]);
	            close(fd[1]);
	            execvp(cmds[i].argv[0], cmds[i].argv);
	            exit(0);
	        }
	        else 
	        {
	            wait(&pipepid);
	            dup2(fd[0], 0); 
	            close(fd[0]);
	            close(fd[1]);
	        }
		}
		// 执行最后一条指令
		execvp(cmds[cmdc-1].argv[0], cmds[cmdc-1].argv);
	}
	else
		wait(NULL);
}

int main()
{
    exec_pipe(3);
    return 0;
}
```



### 题目2：ring.c

1、实现线循环

2、创建N个线程

N个线程构成一个环

主线程向T1发送数据0

T1收到数据后，将数据加1，向T2发送数据1

T2收到数据后，将数据加1，向T3发送数据2

T3收到数据后，将数据加1，向T4发送数据3

 ...

3、创建N个缓冲区

4、每个线程有一个输入缓冲和一个输出缓冲

5、最终的系统结构如下

![](/image/1.png)

6、本程序不能使用任何全局变量，如果使用了全局变量，本题没有得分

```c
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
```





