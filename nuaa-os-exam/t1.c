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