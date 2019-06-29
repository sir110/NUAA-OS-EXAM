#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int fd[2], fd2[2];
	int i, sum, p1, p2;
	pipe(fd);
	pipe(fd2);
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