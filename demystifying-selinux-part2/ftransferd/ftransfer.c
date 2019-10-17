#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 512

int main(int argc, char *argv[])
{
	struct sockaddr_in srv_addr; 	/* server's socket address */
	struct in_addr serv_IP; 	/* servers IP address */
	int csd;			/* listening socket descriptor */
	int fd;				/* file descriptor for waiting file data */
	char buf[BUF_SIZE];		/* buffer to hold data */
	char *client_request;		/* buffer to hold the client request string */
	int client_request_len, clen_network;
	ssize_t count;			/* number of bytes read from socket */

	if (argc != 6) {
		fprintf(stderr, "usage: %s user file serv_port serv_IP destination\n", argv[0]);
		exit(1);
	}
	
	/* create socket */
	if ((csd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Failed to create socket \n");
		exit(2);
	}

	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(atoi(argv[3]));

	if (inet_pton(AF_INET, argv[4], &serv_IP) <= 0) {
		perror("Invalid server IP address");
		exit(3);
	}

	srv_addr.sin_addr = serv_IP;

	/* connect to server */
	if (connect(csd, (struct sockaddr * )&srv_addr, sizeof(srv_addr)) < 0) {
		perror("Failed to connect");
		exit(4);
	}

	/* Write request */
	client_request_len = asprintf(&client_request,"%s %s",argv[1], argv[2]);
	if (!client_request) {
		perror("Failed to allocate client request");
		exit(4);
	}
	clen_network = htonl(client_request_len);
	if (write(csd, &clen_network, 4) < 4) {
		perror("Failed to write client request length");
		exit(4);
	}
	if (write(csd, client_request, client_request_len) < client_request_len) {
		perror("Failed to write client request");
		exit(4);
	}

	/* create file to write data received from server */
	if ((fd = open(argv[5], O_WRONLY|O_TRUNC|O_CREAT, 0666)) < 0) {
		fprintf(stderr, "Unable to open %s for writing: %m\n", argv[5]);
		exit(5);
		
	}

	/* read data from socket and write data to file */
	while ((count = read(csd, buf, BUF_SIZE)) > 0) {
		if (write(fd, buf, count) != count) {
			fprintf(stderr, "Error writing %d bytes to output file: %m\n", count);
			unlink(argv[5]);
			exit(6);
		}
	}

	if (count == -1) {
		perror("Read from socket failed");
		unlink(argv[5]);
		exit(7);
	}

	/* clean up */
	close(fd);
	close(csd);

	return 0;
}
