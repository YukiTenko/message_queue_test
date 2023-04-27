// ftok(): is use to generate a unique key.
// msgget(): either returns the message queue identifier for a newly created message queue or returns the identifiers for a queue which exists with the same key value.
// msgsnd(): Data is placed on to a message queue by calling msgsnd().
// msgrcv(): messages are retrieved from a queue.
// msgctl(): It performs various operations on a queue. Generally it is use to destroy message queue.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int len;
   key_t key;
   struct msqid_ds msqID_DS;
//    system("touch msgq.txt");
   
   if ((key = ftok("test1totest2", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }
   
   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to send messages.\n");
   printf("Enter lines of text, ^D to quit:\n");
   buf.mtype = 1; /* we don't really care in this case */
   
   while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {

      len = strlen(buf.mtext);

      /* remove newline at end, if it exists */
      if (buf.mtext[len-1] == '\n') 
        buf.mtext[len-1] = '\0';

      if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
        perror("msgsnd");

       msgctl(msqid, IPC_STAT, &msqID_DS);
       printf("Number of message in queue: %ld, \n", msqID_DS.msg_qnum);
   }
   strcpy(buf.mtext, "end");
   len = strlen(buf.mtext);
   if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
   perror("msgsnd");
   
   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done sending messages.\n");
   return 0;
}