#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#define QUEUE_NAME  "/test1totest2" /* Queue name. */
#define QUEUE_PERMS 0644
#define QUEUE_MAXMSG  5 /* Maximum number of messages. */
#define QUEUE_MSGSIZE 99999 /* Length of message. */

/* The consumer is faster than the publisher. */
#define QUEUE_POLL_CONSUMER ((struct timespec){1, 0})

int main() {
	mqd_t mq;
	struct timespec poll_sleep;
	struct mq_attr attr;
	unsigned int prio;
	ssize_t bytes_read;
	char buffer[QUEUE_MSGSIZE + 1];
   
	/* Create the message queue. The queue reader is NONBLOCK. */
   attr.mq_flags = 0;
   attr.mq_maxmsg = QUEUE_MAXMSG;
   attr.mq_msgsize = QUEUE_MSGSIZE;
   attr.mq_curmsgs = 0;
   attr.__pad[0] = 0;
   attr.__pad[1] = 0;
   attr.__pad[2] = 0;
   attr.__pad[3] = 0;
	mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, QUEUE_PERMS, &attr);
	if(mq < 0) {
		fprintf(stderr, "[CONSUMER]: Error, cannot open the queue: %s.\n", strerror(errno));
		exit(1);
	}
	
	printf("[CONSUMER]: Queue opened, queue descriptor: %d.\n", mq);
		
	while(1) {
		memset(buffer, 0x00, sizeof(buffer));
		bytes_read = mq_receive(mq, buffer, QUEUE_MSGSIZE, &prio);
		if(bytes_read >= 0) {
			printf("[CONSUMER]: Received message: \"%s\"\n", buffer);
		} else {
			//printf("[CONSUMER]: No messages yet.\n");
			poll_sleep = QUEUE_POLL_CONSUMER;
			nanosleep(&poll_sleep, NULL);
		}
		
		fflush(stdout);
	}

	printf("[CONSUMER]: Cleanup...\n");
	mq_close(mq);
	mq_unlink(QUEUE_NAME);
}
