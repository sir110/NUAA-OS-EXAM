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