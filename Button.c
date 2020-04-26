
#include "util.h"

int pid;

key_t key;
int msgid;

int stato;

int main(int argc, char* argv[])
{
	double tempostd, tempoatt;
	clock_t inizio;
	
  	pid = getpid(); 

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
        msgsnd(msgid, &message, sizeof(message), 0);
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
        key_ppid = ftok("/tmp/ipc/mqueues", ppid);
        msgid_ppid = msgget(key_ppid, 0666 | IPC_CREAT);
        message.mesg_type = 1;
        msgsnd(msgid_ppid, &message, sizeof(message), 0);
      
        msgctl(msgid, IPC_RMID, NULL);
        exit(0);
      }

    }
    return 0;
}
