### 一、文件读写编程题目

#### myecho.c

* myecho.c的功能与系统echo程序相同

* 接受命令行参数，并将参数打印出来，例子如下：

```shell
$ ./myecho x
x
$ ./myecho a b c
a b c
```

* 实现思路：argv[]数组中依次存放所有字串，不断循环，输出命令后的所有字符即可
* 代码实现

```c
#include <stdio.h>
//argc表示文件运行时 以空格为间隔的所有参数个数
//argv[]数组中依次存放所有字串
int main(int argc, char *argv[])
{
    int i = 1;
    //不断循环，输出命令后的所有字符
    while(i < argc)
    {
        printf("%s ", argv[i]);
        i++;
    }
    printf("\n");
    return 0;
}
```



#### mycat.c

- mycat.c的功能与系统cat程序相同
- mycat将指定的文件内容输出到屏幕，例子如下：
- 要求使用系统调用open/read/write/close实现

```shell
$ cat /etc/passwd 
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
...
$ ./mycat /etc/passwd 
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
...
```

* 实现思路：打开argv[1]指向的文件，循环读取打印到屏幕上
* 实现如下

```c
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   	char buf;
    // 打开文件，循环读取
    int fd = open(argv[1], O_RDONLY);
    int count = read(fd, &buf, sizeof(buf));
    while(count != 0)
    {
       // 循环读取后进行输出
       printf("%c", buf);
       count = read(fd, &buf, sizeof(buf));
    }
    close(fd); // 关闭文件
    return 0;
}
```





#### mycp.c

- mycp.c的功能与系统cp程序相同
- 将源文件复制到目标文件，例子如下：
- 要求使用系统调用open/read/write/close实现

```shell
$ cat /etc/passwd
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
...
$ ./mycp /etc/passwd passwd.bak 
$ cat passwd.bak
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
...
```

* 实现思路：只读方式打开argv[1]指向的文件，写方式打开argv[2]指向的文件，循环读取并写入文件
* 实现如下

```c
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char buf = NULL;
    // 打开文件，循环读取
    int fd = open(argv[1], O_RDONLY); //O_RDONLY以只读方式打开
    int fd2 = open(argv[2], O_WRONLY | O_TRUNC); //O_TRUNC如果文件存在，将文件的长度截断为0
    int count = read(fd, &buf, sizeof(buf));
    while(count != 0)
    {
        // 边读取，边写入
        write(fd2, &buf, sizeof(buf));
        count = read(fd, &buf, sizeof(buf));
    }
    close(fd); // 关闭打开的文件
    close(fd2);
    return 0;
}
```







### 二、多进程题目

#### mysys.c

实现函数mysys，用于执行一个系统命令，要求如下

- mysys的功能与系统函数system相同，要求用进程管理相关系统调用自己实现一遍
- **使用fork/exec/wait系统调用实现mysys**
- 不能通过调用系统函数system实现mysys
- 测试程序

```c
#include <stdio.h>

void mysys(char *command)
{
    实现该函数，该函数执行一条命令，并等待该命令执行结束
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
```

- 测试程序的输出结果

```
--------------------------------------------------
HELLO WORLD
--------------------------------------------------
bin    core  home	     lib	 mnt   root  snap  tmp	vmlinuz
boot   dev   initrd.img      lost+found  opt   run   srv   usr	vmlinuz.old
cdrom  etc   initrd.img.old  media	 proc  sbin  sys   var
--------------------------------------------------
```

* 实现思路：在mysys()中创建子进程，然后将字符串中的命令使用execl()装入程序。execl()功能是将当前进程的地址空间的内容全部清空，将path指定的可执行程序的代码和数据装入到当前进程的地址空间。
* 实现如下

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

void mysys(char *command)
{
    pid_t pid;
    // 创建一个子进程，父子进程并发运行
    pid = fork();
    if(pid == 0)
    {
        // 装入程序，将当前进程的地址空间的内容全部清空
        execl("/bin/sh", "sh", "-c", command, NULL);
    }
    else
    {
        // 等待子进程结束
        wait(NULL);
    }
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
```



#### sh1.c

* 该程序读取用户输入的命令，调用函数mysys(上一个作业)执行用户的命令，示例如下

```shell
# 编译sh1.c
$ cc -o sh1 sh1.c

# 执行sh1
$ ./sh 

# sh1打印提示符>，同时读取用户输入的命令echo，并执行输出结果
> echo a b c
a b c

# sh1打印提示符>，同时读取用户输入的命令cat，并执行输出结果
> cat /etc/passwd
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
```

- 请考虑如何实现内置命令cd、pwd、exit
- 实现思路：

（1）在main()函数中读取输入的命令字符串

（2）parseline(buf, argv)函数将一维数组转化为二维数组

（3）buildinCommand(char **argv)函数执行内置命令

（4）在mysys()中创建子进程装入程序

- 实现如下

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

#define BUFFER_SIZE 1024
int parseline(char *buf, char **argv);
int buildinCommand(char **argv);
int main (int argc, char *argv[]) 
{
	// buf用于缓存输入的命令
    char buf[BUFFER_SIZE] = {0};
    int n;
    while(1) 
    {
        printf("> ");
        fflush(stdout);//清除读写缓冲区
        int i;
        for (i = 0; i < BUFFER_SIZE; i++) 
            buf[i] = 0;
        // 直接使用文件描述符0读取输入的字符
        if ((n = read(0, buf, BUFFER_SIZE)) < 0) 
        {
            printf("wrong in input .\n");
        }
        // 命令运行
        mysys(buf);
    }
    return 0;
}

void mysys(char *cmd) 
{
    char *argv[BUFFER_SIZE] = {NULL};
    char buf[BUFFER_SIZE] = {0};
    int j;
    for (j = 0; j < BUFFER_SIZE; j++) // 清空argv
        argv[j] = NULL;
    strcpy(buf, cmd);
    parseline(buf, argv); // 将buf转化为argv
    if (argv[0] == NULL) 
        return ;
    if (buildinCommand(argv)) // 内置命令
        return;
    pid_t pid = fork(); // 创建子进程
    if (pid == 0) 
        execvp(argv[0], argv); // 装入程序
    else 
        wait(NULL); // 父进程等待子进程结束
}

int parseline(char *buf, char **argv)
{ 
    if (strcmp(buf, "\n") == 0) 
        return 0;
    char *p = NULL;
    int argc = 0;
    p = strtok(buf, " \n");  // 分解字符串为一组字符串
    while (p != NULL) 
    {
        argv[argc++] = p;
        p = strtok(NULL, " \n"); // 再次调用要把s设成NULL
    }
    argv[argc] = NULL;
    return 0;
}
// 实现内置命令
int buildinCommand(char **argv)
{
    if(strcmp(argv[0],"exit") == 0)
        exit(0);
    if(strcmp(argv[0],"cd") == 0)
    {
        if(chdir(argv[1])) // chdir更改当前工作目录
            printf("cd: no such file or directory: %s\n", argv[1]);
        return 1;
    }
    if(strcmp(argv[0], "pwd") == 0)
    {
        char buf[BUFFER_SIZE];
        printf("%s\n",getcwd(buf, sizeof(buf))); // getcwd获取当前工作目录
        return 1;
    }
    return 0;
}
```



#### sh2.c

实现shell程序，要求在第1版的基础上，添加如下功能

* 实现文件重定向

```shell
# 执行sh2
$ ./sh2

# 执行命令echo，并将输出保存到文件log中
> echo hello >log

# 打印cat命令的输出结果
> cat log
hello
```

* 实现思路：

（1）代码结构与sh1.c相同

（2）在parseline函数中读取重定向符，并打开文件，使标准输出重定向至打开的文件

* 实现代码

```c
// 在parseline函数中修改如下，其他跟sh1.c相同
int parseline(char *buf, char **argv)
{ 
    if (strcmp(buf, "\n") == 0) 
    {
        return 0;
    }
    char *p = NULL;
    char *filename = NULL;
    int argc = 0;
    p = strtok(buf, " \n"); // 分割字符串
    while (p != NULL) 
    {
        if (p[0] == '>') // 重定向输出至文件
        { 
            // 如果p[1]非空，那么下一个就是文件名
            if(p[1] != ' ' && p[1] != '\n' && p[1] != '\0') 
            { 
                filename = p+1;
            } 
            else 
            {
                filename = strtok(NULL, "> \n");
            }
            if (filename != NULL) 
            {
                int fd; // 打开重定向文件
                fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
                if (fd < 0) // 检查打开文件是否失败
                {
                    printf("File open error.\n");
                    return -1;
                }
                dup2(fd, 1);  // 重定向输出至文件，文件描述符1（标准输出）指向文件描述符fd
                break;
            } 
            else 
            {
                printf("Error redirect file.\n");
            }
        } 
        else 
        {
            argv[argc++] = p;
            p = strtok(NULL, " \n");
        }
    }
    argv[argc] = NULL;
    return 0;
}
```





#### sh3.c

实现shell程序，要求在第2版的基础上，添加如下功能

* 实现管道

```shell
# 执行sh3
$ ./sh3

# 执行命令cat和wc，使用管道连接cat和wc
> cat /etc/passwd | wc -l
```

* 考虑如何实现管道和文件重定向

```shell
$ cat input.txt
3
2
1
3
2
1
$ cat <input.txt | sort | uniq | cat >output.txt
$ cat output.txt
1
2
3
```

* 实现思路：

（1）代码结构与sh1.c相仿

（2）修改mysys函数，分割字符串，扫描内置命令

（3）扫描字符串，打开重定向符指向的文件，需使标准输入重定向至打开的文件

（4）扫描到管道标志时，创建子进程执行管道标志前的命令

（5）扫描命令完成后，执行最后一条命令，并恢复标准输入输出标识符，关闭打开的文件

* 实现代码：

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
int parseline(char *buf, char **argv); // 分割字符串
int buildinCommand(char **argv); // 内建命令

void mysys(char *cmd) 
{
    char *argv[BUFFER_SIZE] = {NULL};
    char buf[BUFFER_SIZE] = {0};
    int j;
    for (j = 0; j < BUFFER_SIZE; j++) 
        argv[j] = NULL;
    strcpy(buf, cmd);
    parseline(buf, argv); // 分割字符串
    if (argv[0] == NULL) 
    	return ;
    if (buildinCommand(argv)) // 内建命令
    	return;
    pid_t pid = fork(); // 创建子进程
    if (pid == 0) 
    {
        // > 输出重定向
        int i = 0;
        char** tmpcmd = (char**)malloc(sizeof(char*) * 20); 
        int tcmdp = 0;  // tmpcmd指针
        int fd[512] = {0};
        int recur = -1;
        int sfd = dup(STDOUT_FILENO);
        // < 输入重定向
        int openfd[512] = {0};
        int open_recur = -1;
        // | 管道
        int forkfd[2];
        // 实现命令ll
        if(strcmp(argv[0], "ll") == 0) 
        {
            tmpcmd[0] = "ls";
            tmpcmd[1] = "-l";
            i++;
            tcmdp += 2;
        }
        while(argv[i] != NULL) 
        {
            // 打开>后面的文件
            if(strcmp(argv[i], ">") == 0) 
            {
                if(strcmp(argv[i+1], ">") == 0) 
                {
                    printf("syntax error near unexpected token '>'\n");
                    return;
                }
                // 打开文件
                recur++;
                fd[recur] = open(argv[i+1], O_CREAT|O_RDWR|O_TRUNC, 0755);
                // i加2指向下一个命令
                i+=2;
            }
            else if(strcmp(argv[i], "<") == 0) 
            {
                if(strcmp(argv[i+1], "<") == 0) 
                {
                    printf("syntax error near unexpected token '<'\n");
                    return;
                }
                // 打开文件
                open_recur++;
                openfd[open_recur] = open(argv[i+1], O_RDONLY);
                dup2(openfd[open_recur], 0); // 输入重定向
                // i加2指向下一个命令
                i+=2;
            }
            else if(strcmp(argv[i], "|") == 0) 
            {
                pipe(forkfd); // 管道
                int pipepid = fork();

                if(pipepid == 0) 
                {
                    tmpcmd[tcmdp] = NULL;
                    dup2(forkfd[1], 1);
                    close(forkfd[0]);
                    close(forkfd[1]);
                    execvp(tmpcmd[0], tmpcmd);
                    exit(0);
                }
                else 
                {
                    wait(&pipepid);
                    dup2(forkfd[0], 0); 
                    close(forkfd[0]);
                    close(forkfd[1]);
                    i++;
                    tcmdp = 0;
                    continue;
                }
            }
            // 最后的命令
            else 
            {
                tmpcmd[tcmdp] = (char*)malloc(sizeof(char) * 512);
                strcpy(tmpcmd[tcmdp], argv[i]);
                i++;
                tcmdp++;
            }
        }
        tmpcmd[tcmdp] = NULL;
        if(recur != -1) 
        {
            dup2(fd[recur], 1); // 重定向输出
        }
        if(execvp(tmpcmd[0], tmpcmd) < 0) // 执行最后一条命令
        {
            dup2(sfd, STDOUT_FILENO);
            printf("%s: command not found\n", argv[0]);
            return;
        }
        while(recur > 0) // 关闭输出文件
        {
            close(fd[recur]);
            recur--;
        }
        while(open_recur > 0) // 关闭输入文件
        {
            close(openfd[open_recur]);
            open_recur--;
        }
        close(fd[0]);
        close(openfd[0]);
        dup2(sfd, STDOUT_FILENO);   // 恢复输出
    }
    else 
        wait(NULL);
}
```





### 三、多线程题目

#### pi1.c

使用2个线程根据莱布尼兹级数计算PI

- 莱布尼兹级数公式: 1 - 1/3 + 1/5 - 1/7 + 1/9 - ... = PI/4
- 主线程创建1个辅助线程
- 主线程计算级数的前半部分
- 辅助线程计算级数的后半部分
- 主线程等待辅助线程运行結束后，将前半部分和后半部分相加
- 实现思路：

（1）主线程计算级数的前半部分，辅助线程计算级数的后半部分

（2）函数计算时用pow()来确定符号

（3）主线程等待辅助线程运行結束后，将前半部分和后半部分相加

- 实现如下

```c
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define N 1000000
double sum1 = 0, sum2 = 0;

int main (int argc, char *argv[]) 
{
    pthread_t tid;
    pthread_create(&tid, NULL, worker, NULL); // 创建子进程
    master();
    pthread_join(tid, NULL); // 等待子进程结束
    double pi = (sum1 + sum2) * 4; // 计算pi值
    printf("PI = %lf\n", pi);
    return 0;
}
// 辅助线程计算级数的后半部分
void *worker(void *arg) 
{
    int i;    
    for (i = N + 1; i <= 2 * N - 1; i += 2)
        sum1 += pow(-1 , (i-1) / 2) * 1/i; // pow()用来确定符号
}
// 主线程计算级数的前半部分
void master() 
{
    int i;
    for (i = 1; i <= N - 1; i += 2 ) 
        sum2 += pow(-1 , (i-1) / 2) * 1/i;
}
```

编译命令

```shell
cc -o pi1 pi1.c -lpthread -lm
```



#### pi2.c

使用N个线程根据莱布尼兹级数计算PI

- 与上一题类似，但本题更加通用化，能适应N个核心，需要使用线程参数来实现
- 主线程创建N个辅助线程
- 每个辅助线程计算一部分任务，并将结果返回
- 主线程等待N个辅助线程运行结束，将所有辅助线程的结果累加
- 实现思路：

（1）主线程创建N个辅助线程，每个进程计算一部分任务

（2）每个进程计算i,i+16,i+16*2,.....，<=N，并返回计算的值

（3）计算时用pow()确定符号，pow()包含在math.h中

（4）主线程等待辅助线程运行结束，并将结果累加

- 实现如下

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define N 1000000
#define N_CPU_CORE 8

struct param {
    int start;
};
struct result {
    double sum;
};
int main (int argc, char *argv[]) 
{
    pthread_t tid[N_CPU_CORE];
    struct param params[N_CPU_CORE]; // 定义每个子进程的结构体
    struct param *param;
    struct result *result;
    int i;
    double pi = 0;
    for (i = 0; i < N_CPU_CORE; i++) 
    {   //pthread 1  2  3  4    5  6   7   8
        //compute 1  3  5  7    9  11  13  15 
        //        17 19 ...
        param = &params[i];
        param->start = 2 * i + 1; // 传入开始的计算值
        pthread_create(&tid[i], NULL, compute, param); // 辅助线程计算一部分任务，并将结果返回
    }
    for (i = 0; i < N_CPU_CORE; i++) 
    {
        pthread_join(tid[i], (void **)&result); // 等待每个辅助进程返回值
        pi += result->sum; // 求和
        free(result);
    }
    printf("PI = %lf\n", pi * 4);
    return 0;
}
void *compute(void *arg) 
{
    struct param *param;
    struct result *result;
    double sum = 0;
    int i;
    param = (struct param*) arg;
    for (i = param->start; i <= N; i += (2 * N_CPU_CORE)) 
    { // 每个进程计算i,i+16,i+16*2,.....，<=N
        sum += pow(-1, (i-1) / 2) * 1/i; // pow()确定符号
    }
    result = (struct result *)malloc(sizeof(struct result));
    result->sum = sum;
    return result; // 返回计算的值
}
```

编译命令

```shell
cc -o pi2 pi2.c -lpthread -lm
```



#### sort.c

多线程排序

- 主线程创建一个辅助线程
- 主线程使用选择排序算法对数组的前半部分排序
- 辅助线程使用选择排序算法对数组的后半部分排序
- 主线程等待辅助线程运行結束后,使用归并排序算法归并数组的前半部分和后半部分
- 实现思路：

（1）主线程对前半部分排序，辅助线程对后半部分排序，主线程等待辅助线程运行结束

（2）merge()连接两个已排序的数组，连接时需考虑长度奇偶性

（3）最后输出排序好的数组

- 实现如下

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

int len=0;
// 选择排序算法
void sort(void *p)
{
    int *x = (int*)p;
    int i, j, k;
    for(i = 0; i < len/2-1; i++)
    {
        k = i; // 记录待选择排序的下标
        for(j = i+1; j < len/2; j++)
        {
            if(x[j] > x[k]) // 判断是否有大于的值，如有那么换成较大值的下标
                k = j;
        }
        if(k != i) // 如果不等于原先值，那么交换
        {
            x[i] = x[i] ^ x[k];
            x[k] = x[i] ^ x[k];
            x[i] = x[i] ^ x[k];
        }
    }
}
// merge连接两个已排序的数组
void merge(int *a, int start, int mid, int end)
{
    int *tmp = (int *)malloc((end - start + 1) * sizeof(int)); // 申请缓存区
    int i = start;
    int j = mid+1;
    int k = 0;
    while(i <= mid && j<=end)
    { // 合并两个排序的数组，tmp存入较大的值
        if(a[i] >= a[j])
            tmp[k++] = a[i++];
        else
            tmp[k++] = a[j++];
    }
    while(i <= mid) // 存入接下来的值
        tmp[k++] = a[i++];
    while(j <= end)
        tmp[k++] = a[j++];
    for(i = 0; i < k; i++) // 将缓存值复制回原来的数组
    {
        a[start+i] = tmp[i];
    }
    free(tmp);
}

int main()
{
    int array[100]={100,123,456,789,963,852,741,1,56,789456,123,555,9}; // 测试数组
    int i=0;
    while(array[i]) // 记录长度
    {
        len++;
        i++;
    }
    printf("%d\n", len);
    pthread_t tid;
    sort(array); // 主线程对数组的前半部分排序，辅助进程对数组的后半部分排序
    pthread_create(&tid, NULL, (void*)&sort, (void*)(array+len/2));
    pthread_join(tid, NULL); // 等待线程结束
    i = 0;
    while(i < len)
    {
        printf("%d ", array[i++]);
    }
    printf("\n");
    if(len%2 != 0) // 长度为奇数，需归并两次
    {
        merge(array, 0, len/2-1, len-2); // 归并len-1
        merge(array, 0, len-2, len-1); // 归并最后一个数
    }
    else
        merge(array, 0, len/2-1, len-1); // 若为偶数，可直接归并
    i = 0;
    while(i < len)
        printf("%d ", array[i++]); // 输出已排序的数组
    printf("\n");
    return 0;
}
```



#### pc1.c

使用条件变量解决生产者、计算者、消费者问题

- 系统中有3个线程：生产者、计算者、消费者
- 系统中有2个容量为4的缓冲区：buffer1、buffer2
- 生产者生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符，放入到buffer1
- 计算者从buffer1取出字符，将小写字符转换为大写字符，放入到buffer2
- 消费者从buffer2取出字符，将其打印到屏幕上

* 实现思路：

（1）设置两个容量为4的缓冲区，变量out1/2为共享缓冲去的读指针，变量in1/2为共享缓冲去的写指针

（2）实现缓冲区操作、判断函数

（3）生产者线程与消费者线程需要互斥，需要设置互斥量互斥访问共享变量in/out

（4）生产者线程与消费者线程需要同步，需要设置条件变量同步

（5）主函数为生产者线程，创建消费者和计算者子进程，三者并发运行

* 实现如下

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int buffer1[4], buffer2[4];
int in1=0, out1=0; // 变量out为共享缓冲去的读指针，变量in为共享缓冲去的写指针
int in2=0, out2=0;
int buffer1_is_empty() // in指针和out指针相同时，缓冲区为空
{
    return in1 == out1;
}
int buffer1_is_full() // in指针和out指针相邻时，缓冲区为满
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
int get_item1() // get_item获取out指针指向的元素，同时移动out指针指向下一项
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
void put_item1(int item) // put_item将元素放置在in指针指向的位置，同时移动in指针指向下一项
{
    buffer1[in1] = item;
    in1 = (in1 + 1) % 4;
}
void put_item2(int item)
{
    buffer2[in2] = item;
    in2 = (in2 + 1) % 4;
}

pthread_mutex_t mutex1, mutex2; // 生产者线程与消费者线程需要互斥，需要互斥访问共享变量in/out
pthread_cond_t empty1, full1, empty2, full2; // 生产者线程与消费者线程需要同步

void produce() // 生产者
{
    int i;
    int item;
    for(i = 0; i < 8; i++)
    {
        pthread_mutex_lock(&mutex1);
        while(buffer1_is_full()) // 如果buffer_is_full为真，缓冲区为满
            pthread_cond_wait(&empty1, &mutex1); // 生产者等待条件变量
        item = 'a' + i; 
        put_item1(item); // 生产者生产一个数据
        printf("produce item: %c\n", item);
        pthread_cond_signal(&full1); // 释放一个满缓冲区，唤醒等待满缓冲区的消费者
        pthread_mutex_unlock(&mutex1);
    }
}
void calculate() // 计算者
{
    int i, item;
    for(i=0; i<8; i++)
    {
        pthread_mutex_lock(&mutex1); // 访问互斥量先加锁
        while(buffer1_is_empty())
            pthread_cond_wait(&full1, &mutex1);
        item = get_item1(); // 从buffer1取出字符
        item -= 32; // 将小写字符转换为大写字符
        pthread_cond_signal(&empty1);
        pthread_mutex_unlock(&mutex1);

        pthread_mutex_lock(&mutex2); // 访问互斥量先加锁
        while(buffer2_is_full())
            pthread_cond_wait(&empty2, &mutex2);
        put_item2(item); // 将字符放入buffer2
        pthread_cond_signal(&full2);
        pthread_mutex_unlock(&mutex2);
    }
}
void consume() // 消费者
{
    int i, item;
    for(i = 0; i < 8; i++)
    {
        pthread_mutex_lock(&mutex2);
        while(buffer2_is_empty()) // 如果buffer_is_empty为真，缓冲区为空
            pthread_cond_wait(&full2, &mutex2); // 消费者等待条件变量
        item = get_item2(); // 消费者取走一个数据
        printf("    consume item: %c\n", item);
        pthread_cond_signal(&empty2); // 释放一个空缓冲区，唤醒等待空缓冲区的生产者
        pthread_mutex_unlock(&mutex2);
    }
}

int main()
{
    pthread_t tid[2];
    pthread_mutex_init(&mutex1, NULL); // 初始化mutex互斥量
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&empty1, NULL); // 初始化条件变量
    pthread_cond_init(&full1, NULL);
    pthread_cond_init(&empty2, NULL);
    pthread_cond_init(&full2, NULL);
    pthread_create(&tid[0], NULL, (void *)&consume, NULL); // 创建消费者线程
    pthread_create(&tid[1], NULL, (void *)&calculate, NULL); // 创建计算者线程 
    produce(); // 主函数为生产者线程
    int i;
    for(i = 0; i < 2; i++)
        pthread_join(tid[i], NULL); // 调用pthread_join等待线程结束
    return 0; // 结束整个进程
}
```





#### pc2.c

使用信号量解决生产者、计算者、消费者问题

- 功能和前面的实验相同，使用信号量解决
- 实现思路同上：还需实现信号量的初始化、等待阻塞和唤醒函数
- 实现如下

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int buffer1[4], buffer2[4];
int in, out; // 变量out为共享缓冲去的读指针，变量in为共享缓冲去的写指针
int comp2, cons;
int buffer1_is_empty() // in指针和out指针相同时，缓冲区为空
{
    return in == out;
}
int buffer1_is_full() // in指针和out指针相邻时，缓冲区为满
{
    return (in + 1) % 4 == out; // 通过求模保证in指针处于数组边界内
}
int buffer2_is_empty()
{
    return comp2 == cons;
}
int buffer2_is_full()
{
    return (cons + 1) % 4 == comp2;
}
int get_item1() // get_item获取out指针指向的元素，同时移动out指针指向下一项
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
void put_item1(int item) // put_item将元素放置在in指针指向的位置，同时移动in指针指向下一项
{
    buffer1[in] = item;
    in = (in + 1) % 4;
}
void put_item2(int item)
{
    buffer2[comp2] = item;
    comp2 = (comp2 + 1) % 4;
}

typedef struct{ // 使用条件变量实现信号量sema_t，value记录了信号量的值
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
    while(sema->value <= 0) // 如果信号量的值小于等于0，则等待条件变量!
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--; // 将信号量的值减一
    pthread_mutex_unlock(&sema->mutex);
}
void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value; // 将信号量的值加一
    pthread_cond_signal(&sema->cond); // 唤醒等待条件变量的线程
    pthread_mutex_unlock(&sema->mutex);
}
sema_t mutex1; // 用于互斥访问共享缓冲区变量in/out
sema_t empty1; // 用于线程同步
sema_t full1;
sema_t mutex2; // 用于互斥访问共享缓冲区变量comp2/cons
sema_t empty2;
sema_t full2;

void produce() // 生产者
{
    int i, item;
    for(i = 0; i < 8; i++)
    {
        sema_wait(&empty1); // empty1的数值表示空buffer的数量
        sema_wait(&mutex1); // 用于对共享变量进行互斥访问
        item = 'a' + i;
        put_item1(item);
        printf("produce item: %c\n", item); // 从缓冲区中获取数据并打印
        sema_signal(&mutex1);
        sema_signal(&full1); // 生产者线程生产一个数据后，产生一个新的满buffer，释放信号量full1
    }
}
void calculate() // 计算者
{
    int i, item;
    for(i=0; i<8; i++)
    {
        sema_wait(&full1);
        sema_wait(&mutex1);
        item = get_item1(); // 从buffer1取出字符
        item -= 32; // 将小写字符转换为大写字符
        sema_signal(&mutex1);
        sema_signal(&empty1);
        
        sema_wait(&empty2);
        sema_wait(&mutex2);
        put_item2(item); // 将字符放入buffer2
        sema_signal(&mutex2);
        sema_signal(&full2);
    }
}
void consume() // 消费者
{
    int i, item;
    for(i=0; i<8; i++)
    {
        sema_wait(&full2); // full2的数值表示满buffer的数量
        sema_wait(&mutex2); // 用于对共享变量进行互斥访问
        item = get_item2(); // 从缓冲区中获取数据并打印
        printf("    consume item: %c\n", item); 
        sema_signal(&mutex2); // 释放锁
        sema_signal(&empty2); // 消费者线程取走一个数据后，产生一个新的空buffer，释放信号量empty2
    }
}

int main()
{
    pthread_t tid[2];
    sema_init(&mutex1, 1); // 初始化信号量
    sema_init(&mutex2, 1);
    sema_init(&empty1, 3);
    sema_init(&full1, 0);
    sema_init(&empty2, 3);
    sema_init(&full2, 0);
    pthread_create(&tid[0], NULL, (void *)&consume, NULL); // 创建消费者线程
    pthread_create(&tid[1], NULL, (void *)&calculate, NULL); // 创建计算者线程
    produce(); // 主线程作为生产者，执行produce函数
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    return 0;
}
```

* 运行结果

```shell
$ ./pc2
produce item: a
produce item: b
produce item: c
    consume item: A
    consume item: B
    consume item: C
produce item: d
produce item: e
produce item: f
    consume item: D
    consume item: E
    consume item: F
produce item: g
produce item: h
    consume item: G
    consume item: H
```



#### ring.c

创建N个线程，它们构成一个环

- 创建N个线程：T1、T2、T3、… TN
- T1向T2发送整数1
- T2收到后将整数加1
- T2向T3发送整数2
- T3收到后将整数加1
- T3向T4发送整数3
- …
- TN收到后将整数加1
- TN向T1发送整数N

* 实现思路：

（1）主函数创建线程，并传送整数result至线程

（2）被创建线程收到后将整数加1，传送到下一个进程

（3）共享变量需加锁进行互斥访问，加一后唤醒阻塞在条件变量上的所有线程

* 实现如下

```c
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 10
pthread_mutex_t mutex;
pthread_cond_t cond;

void *fun(void *arg)
{
    pthread_mutex_lock(&mutex); // 对共享变量进行互斥访问
    int *p = (int*)arg;
    printf("Thread:T%d   values:%d\n", (*p) + 1, (*p) + 1); // 输出进程的整数值
    (*p)++;
    pthread_cond_broadcast(&cond); // 唤醒阻塞在条件变量上的所有线程
    pthread_mutex_unlock(&mutex); // 释放锁
    return (void*)p; // 返回加一后的值
}

int main()
{
    pthread_t tid[N];
    int *result = (int*)malloc(sizeof(int));
    *result = 0;
    pthread_mutex_init(&mutex, NULL); // 初始化互斥量
    pthread_cond_init(&cond, NULL); // 初始化条件变量
    int i = 0, count = 12;
    while(count--)
    {
        pthread_create(&tid[i], NULL, fun, (void*)result); // 创建线程，传送整数result
        pthread_join(tid[i], (void **)&result);
        i = (i + 1) % N;
    }
    return 0;
}
```

* 运行结果

```shell
Thread:T1   values:1
Thread:T2   values:2
Thread:T3   values:3
Thread:T4   values:4
Thread:T5   values:5
Thread:T6   values:6
Thread:T7   values:7
Thread:T8   values:8
Thread:T9   values:9
Thread:T10   values:10
Thread:T11   values:11
Thread:T12   values:12
...
```









