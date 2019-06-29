#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   char buf;
   int fd = open(argv[1], O_RDONLY);
   int count = read(fd, &buf, sizeof(buf));
   while(count != 0)
   {
      printf("%c", buf);
      count = read(fd, &buf, sizeof(buf));
   }
   return 0;
}

