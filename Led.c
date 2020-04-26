#include "util.h"

int pid;

key_t key, key_P;
int msgid, msgid_P;

int stato;

int main(int argc, char* argv[]) { 

  pid = getpid(); 

  key = ftok("/tmp/ipc/mqueues", pid);
  msgid = msgget(key, 0666 | IPC_CREAT);

  key_P = ftok("/tmp/ipc/mqueues", getppid());
  msgid_P = msgget(key_P, 0666 | IPC_CREAT);
  
  while(1)
  {
    char primo;
    msgrcv(msgid, &message, sizeof(message), 1, 0);
    primo = message.mesg_text[0];
    if(primo=='A') /*inviare stato ON/OFF*/
    {

      message.mesg_type = 1;
      message.mesg_text[0] = '0'+stato;
      msgsnd(msgid_P, &message, sizeof(message), 0); 
    }
    
    if(primo=='B') /*ricevere stato ON/OFF*/
    {
      msgrcv(msgid, &message, sizeof(message), 1, 0);
      stato=atoi(message.mesg_text);
    }

    if(primo=='E')
    {

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

