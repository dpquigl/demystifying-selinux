#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define BUF_SIZE 512
#define ERR -1
#define OK 0

int main(int argc, char* argv[])
{
	struct sockaddr_in srv_addr;	/* server's socket address */
	struct sockaddr_in clnt_addr;	/* client's socket address */
	socklen_t len;
	int ssd;	/* listening socket descriptor */
	int new_ssd;	/* connected socket descriptor */
	int fd;		/* file descriptor for reading file data */
	char *ptr_buf;	/* ptr to buffer */
	char buf[BUF_SIZE];	/* buffer to hold file data */
	char *in_buf;		/* buffer to hold request from client */
	char *user_token, *file_token, *filename;
	ssize_t in_buf_count;
	size_t count_read;	/* number of bytes read from file */
	ssize_t count_write;	/* number of bytes that must be written to socket */
	ssize_t total;	/* total number of bytes that must be written to socket */
	int optval = 1;	/* specifies that socket address should be reused */
	struct sigaction act, oact; /* structures for various signal actions */

	if (argc != 2) {
		fprintf(stderr, "usage: %s serv_port\n", argv[0]);
		exit(1);
	}

	daemon(0,0);

	/*
	 * Send error information to /var/log/messages for daemon
	 */ 
	openlog(argv[0], LOG_PID, LOG_DAEMON);

	/* create socket */
	if ((ssd = socket(PF_INET, SOCK_STREAM, 0)) == -1 ) {
		syslog(LOG_DAEMON|LOG_CRIT, "Failed to create socket: %m");
		exit(1);
	}
	if (setsockopt(ssd, SOL_SOCKET, SO_REUSEADDR, (void *) &optval, sizeof(optval)) < 0) {
		syslog(LOG_DAEMON|LOG_CRIT, "setsockopt(SO_RUSEADDR) failed: %m");
		exit(2);
	}

	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(atoi(argv[1]));
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind the address to the socket */
	if (bind(ssd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) == -1) {
		syslog(LOG_DAEMON|LOG_CRIT, "Error binding server addresses: %m");
		exit(3);
	}

	bzero(&clnt_addr, sizeof(clnt_addr));

	/* set up the server's listening queue */
	if (listen(ssd, 5) == -1) {
		syslog(LOG_DAEMON|LOG_CRIT, "Error establishing backlog: %m");
		exit(4);
	}

	/* ignore the SIGCHLD signal so that zombies
	 * don't develop or are cleaned up.
	 */
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDWAIT;

	if (sigaction(SIGCHLD, &act, &oact) < 0) {
		syslog(LOG_DAEMON|LOG_CRIT, "Error creating the SIGCHLD signal handler: %m");
		exit(5);
	}

	/*
	 * create a concurrent server
	 */
	for (;;) {
		/* accept a new connection */
		if ((new_ssd = accept(ssd, (struct sockaddr *) &clnt_addr, &len)) == -1) {
			syslog(LOG_DAEMON|LOG_CRIT, " Error accepting connection: %m");
			exit(6);
		}

		switch (fork()) {
			case -1:
				{
					/* fork error occurred */
					syslog(LOG_DAEMON|LOG_CRIT, "fork failed: %m");
					exit(7);
					break;
				}

			case 0:
				{
					/* in child process */
					close(ssd); /* close listening socket descriptor */

					if(read(new_ssd, &in_buf_count, 4) < 4) {
						syslog(LOG_DAEMON|LOG_CRIT, "No request read from client: %m");
						exit(8);
					}
					in_buf_count = ntohl(in_buf_count);
					if(in_buf_count < 0) {
						syslog(LOG_DAEMON|LOG_CRIT, "Client Error: %m, %d", in_buf_count);
						exit(9);
					}
					in_buf = malloc(in_buf_count);
					if(!in_buf) {
						syslog(LOG_DAEMON|LOG_CRIT, "Client Error: %m");
						exit(10);

					}
					if(read(new_ssd, in_buf, in_buf_count) < in_buf_count) {
						free(in_buf);
						syslog(LOG_DAEMON|LOG_CRIT, "Client Error: %m couldn't read from stream");
						exit(11);
					}
					user_token = strsep(&in_buf," ");
					file_token = strsep(&in_buf," ");
					
					if(!user_token || !file_token) {
						free(in_buf);
						syslog(LOG_DAEMON|LOG_CRIT, "Client Error: Invalid request string %m");
						exit(12);
					}
					
					asprintf(&filename,"/home/%s/public_ftransfer/%s",user_token,file_token);
					if(!filename) {
						free(in_buf);
						syslog(LOG_DAEMON|LOG_CRIT, "Couldn't form filename from components: %m");
						exit(13);
					}
					/* open the file for reading */
					if ((fd = open(filename, O_RDONLY)) < 0) {
						syslog(LOG_DAEMON|LOG_CRIT, "Unable to open  %s for reading: %m", filename);
						exit(14);
					}

					/*
					 * read file and write data socket; repeat until
					 * all data has been read from the file
					 */
					while ((count_read = read(fd, buf, BUF_SIZE)) > 0) {
						total = count_read;
						ptr_buf = buf;

						while ((count_write = write(new_ssd, ptr_buf, total)) != -1) {
							total -= count_write;
							ptr_buf += count_write;
							if (total == 0)
								break;
						}

						if (count_write == -1) {
							syslog(LOG_DAEMON|LOG_CRIT, "Error writing to socket: %m");
							exit(15);
						}
					}

					/* print a log message */
					syslog(LOG_DAEMON|LOG_INFO, "File Transfer Server: %s was downloaded\n", filename);

					/* clean up */
					close(fd);
					close(new_ssd);
					return(0);
				}
			default:
				{
					/* in parent */
					close(new_ssd); /* close the connected socket descriptor */
					break;
				}
		}	
	} 
}
