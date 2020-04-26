
#include "util.h"

int pid;

key_t key, key_P;
int msgid, msgid_P;

int stato;

int main(int argc, char* argv[])
{
	double tempostd, tempoatt;
	clock_t inizio;
	
  	pid = getpid(); 
	
	key_P = ftok("/tmp/ipc/mqueues", getppid());
  	msgid_P = msgget(key_P, 0666 | IPC_CREAT);

	key = ftok("/tmp/ipc/mqueues", pid);
  	msgid = msgget(key, 0666 | IPC_CREAT);
  
  	tempostd=0.5;
	tempoatt=0.5;
	inizio = clock();

    while(1)
    {
      char primo;
      msgrcv(msgid, &message, sizeof(message),1, 0);
      primo=message.mesg_text[0];
      if(primo=='A') /*inviare stato ON/OFF*/
      {
        if(stato)
        {
          if(tempoatt<(clock()-inizio))
            stato=0;
        }
            
        message.mesg_type = 1;
        message.mesg_text[0] = '0'+stato;
        msgsnd(msgid_P, &message, sizeof(message), 0);
      }

      if(primo=='B') /*ricevere stato ON/OFF*/
      {
	msgrcv(msgid, &message, sizeof(message), 1, 0);

        inizio=clock();
        tempoatt=tempostd; /*tempoatt è sempre >= tempostd*/
        if(atof(message.mesg_text)>tempoatt)
          tempoatt=atof(message.mesg_text);

      }

      if(primo=='E')
      {
	int ppid, msgid_ppid;
	key_t key_ppid;

        ppid =(int) getppid();
        message.mesg_type = 1;
        msgsnd(msgid_P, &message, sizeof(message), 0);
      
        msgctl(msgid, IPC_RMID, NULL);
        exit(0);
      }

    }
    return 0;
}
