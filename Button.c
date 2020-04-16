#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct mesg_buffer { 
    long mesg_type; 
    char mesg_text[100]; 
} message; 

//^^^^^^^^^^^^^^
//util.h

int pid;

volatile int flag_usr1 = 0;
volatile int flag_usr2 = 0;
volatile int flag_term = 0;

void sighandler_int(int sig) {
    if (sig == SIGUSR1) {
        flag_usr1 = 1;
    }
    if (sig == SIGUSR2) {
        flag_usr2 = 1;
    }
    if (sig == SIGTERM) {
        flag_term = 1;
    }
}

key_t key;
int msgid;

int stato;

int main(int argc, char* argv[])
{
  	pid = getpid(); 
  
  	signal(SIGTERM, sighandler_int);
  	signal(SIGUSR1, sighandler_int);
  	signal(SIGUSR2, sighandler_int);

	key = ftok("/tmp/ipc/mqueues", pid);
  	msgid = msgget(key, 0666 | IPC_CREAT);
  
  	double tempostd=0.5, tempoatt=0.5;
	clock_t inizio = clock();

    while(1)
    {
      
      if(flag_usr1) //inviare stato ON/OFF
      {
        flag_usr1=0;
        if(stato)
        {
          if(tempoatt<(clock()-inizio))
            stato=0;
        }
            
        message.mesg_type = 1;
        message.mesg_text[0] = '0'+stato;
        msgsnd(msgid, &message, sizeof(message), 0);
      }

      if(flag_usr2) //ricevere stato ON/OFF
      {
        flag_usr2=0;

        int errlett = msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);
        if(errlett==-1) printf("Messaggio non letto correttamente");

        inizio=clock();
        tempoatt=tempostd; //tempoatt è sempre >= tempostd
        if(atof(message.mesg_text)>tempoatt)
          tempoatt=atof(message.mesg_text);

      }

      if(flag_term)
      {
        flag_term = 0;

        /*int ppid =(int) getppid();
        key_t key_ppid = ftok("/tmp/ipc/mqueues", ppid);
        int msgid_ppid = msgget(key_ppid, 0666 | IPC_CREAT);
        message.mesg_type = 1;
        msgsnd(msgid_ppid, &message, sizeof(message), 0);*/
      
        msgctl(msgid, IPC_RMID, NULL);
        exit(0);
      }

    }
    return 0;
}
