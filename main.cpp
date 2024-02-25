#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define MAXCONNECTION 2048

	void resetb(char *buf, int bufsize);
	int rddata(int fd, char *buf, int bufsize);
	int wrdata(int fd, const char *buf, int bufsize);

	int main(int argc, char *argv[])
		{
			int nb = 0;
			int state = 0;
			int port = 80;
			char buf[BUFSIZE];
			sockaddr_in servaddr4;

			// Создание сокета

				int listenfd = socket(AF_INET, SOCK_STREAM, 0);

				if(listenfd < 0)
					{
						perror("socket");
						return 1;
					}

			// Заполнение структуры адреса сокета

				bzero(&servaddr4, sizeof(servaddr4));
				servaddr4.sin_family = AF_INET;
				servaddr4.sin_addr.s_addr = htonl(INADDR_ANY);
				servaddr4.sin_port = htons(port);

			// Назначение сокету порта и адреса

				if(bind(listenfd, (sockaddr *) &servaddr4, sizeof(servaddr4)) < 0)
					{
						perror("bind");
						return 1;						
					}
			
			// Запуск прослушивания сокета

				if(listen(listenfd, MAXCONNECTION))
					{
						perror("listen");
						return 1;							
					}

			// Обработка входящих соединений и чтение HTTP-запроса

				while(1) {

					// Блокирование сокета в ожидании соединения

						int connfd = accept(listenfd, nullptr, nullptr);

						if(connfd < 0)
							{
								perror("accept");
								return 1;					
							}

					// Чтение HTTP-запроса с сокета
						
						nb = rddata(connfd, buf, BUFSIZE);
						
						if(nb >= 0) wrdata(STDOUT_FILENO, buf, nb);
							else return 1;

					// Проверка HTTP-запроса на наличие строки "GET / HTTP/1.1" 

						if(strstr(buf, "GET / HTTP/1.1") != NULL)
							{
								// Открытие html-файла со стартовой страницей

									int fd = open("./form.html", O_RDONLY);

									if(fd < 0) { perror("open"); return 1; }

								// Чтение html-файла в буфер и отправка его клиенту

									nb = rddata(fd, buf, BUFSIZE);
									
									if(nb >= 0) wrdata(STDOUT_FILENO, buf, nb);
										else return 1;

									nb = wrdata(connfd, buf, nb);

									if(nb >= 0) resetb(buf, BUFSIZE);
										else return 1;

									close(connfd);

									resetb(buf, BUFSIZE);						

									continue;								
							}

					// Проверка HTTP-запроса на наличие строки "GET /favicon.ico HTTP/1.1" 
						
						if(strstr(buf, "GET /favicon.ico HTTP/1.1") != NULL)
							{
								int fd = open("./favicon.html", O_RDONLY);

								if(fd < 0) { perror("open"); return 1; }

								// Чтение html-файла в буфер и отправка его клиенту

									nb = rddata(fd, buf, BUFSIZE);
									
									if(nb >= 0) wrdata(STDOUT_FILENO, buf, nb);
										else return 1;

									nb = wrdata(connfd, buf, nb);

									if(nb >= 0) resetb(buf, BUFSIZE);
										else return 1;

									close(connfd);

									resetb(buf, BUFSIZE);						

									continue;
							}

					// Проверка HTTP-запроса на наличие строки "POST / HTTP/1.1"
						
						if(strstr(buf, "POST / HTTP/1.1") != NULL)
							{
								

								close(connfd);

								resetb(buf, BUFSIZE);						

								continue;
							}
				}				

				return 0;
		}

	void resetb(char *buf, int bufsize)
		{
			for(int i = 0; i < bufsize; ++i) buf[i] = '\0';
		}

	int rddata(int fd, char *buf, int bufsize)
		{
			int ret = read(fd, buf, bufsize);
			
			if(ret < 0)
				{
					perror("read");
					return -1;
				}

			buf[ret] = '\0';

			return ret;
		}


	int wrdata(int fd, const char *buf, int bufsize)
		{
			int ret = write(fd, buf, bufsize);
			
			if(ret < 0)
				{
					perror("read");
					return -1;
				}

			return ret;
		}


