#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 1024

	int main(int argc, char *argv[])
		{
			int ret, fd;
			char buf[BUFSIZE];

			fd = open("./favicon.ico", O_RDONLY);

			if(fd < 0)
				{
					perror("open");
					return 1;
				}

			ret = read(fd, buf, BUFSIZE);

			if(ret < 0)
				{
					perror("read");
					return 1;
				}	

			fd = open("./favicon.html", O_WRONLY);

			lseek(fd, 0, SEEK_END);

			ret = write(fd, buf, ret);

			if(ret < 0)
				{
					perror("write");
					return 1;
				}					
	
			return 0;
		}
