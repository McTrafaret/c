#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BUFLEN 255

int main(int argc, char **argv)
{
	char *buffer = malloc(BUFLEN);
	char *error;
	int opt = 1;
	int sock, port, read_count;
	struct sockaddr_in addr;
	struct in_addr custom_ip;
	socklen_t len = sizeof(addr);

	if(argc < 3)
	{
		printf("Usage:\necho_client <ip> <port>");
		return 1;
	}
	port = strtol(argv[2], &error, 10);
	if(!*argv[2] || *error)
	{
		fprintf(stderr, "Invalid port number\n");
		return 1;
	}
	if(!inet_aton(argv[1], &custom_ip))
	{
		fprintf(stderr, "Invalid ip address\n");
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
	{
		perror("socket");
		return 2;
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	printf("Succesfully created socket\n");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = custom_ip;
	if(-1 == connect(sock, (const struct sockaddr*) &addr, len))
	{
		perror("connect");
		return 3;
	}
	getsockname(sock, (struct sockaddr*) &addr, &len);
	printf("Succesfully connected to %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	read_count = read(sock, buffer, BUFLEN);
	buffer[read_count] = '\0';
	printf("%s\n", buffer);
	for(;;)
	{
		read_count = read(STDIN_FILENO, buffer, BUFLEN);
		write(sock, buffer, read_count);
	}
}
