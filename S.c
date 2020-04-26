#include "util.h"

int pid_L, pid_T, pid, msgid_T, msgid_L, msgid_C, msgid, msgid_P;
char pipe_L[MAX_BUFF_SIZE], pipe_T[MAX_BUFF_SIZE];

void aziona_T(char azione)
{
  switch(azione)
  {
      case 'A':
      {
          message.mesg_type=1;
          message.mesg_text[0]='A';
          msgsnd(msgid_C, &message, sizeof(message), 0);
      }        
      break;
      case 'B':
      {
          message.mesg_type=1;
          message.mesg_text[0]='B';
          msgsnd(msgid_C, &message, sizeof(message), 0);
      }  
      break;
      case 'C':
      {
          message.mesg_type=1;
          message.mesg_text[0]='C';
          msgsnd(msgid_C, &message, sizeof(message), 0);
      }  
      break;
      case 'D':
      {
          message.mesg_type=1;
          message.mesg_text[0]='D';
          msgsnd(msgid_C, &message, sizeof(message), 0);
      }  
      break;
  }
}

int chiedi_stato_L()
{
  message.mesg_type=1;
  message.mesg_text[0]='A';
  msgsnd(msgid_L, &message, sizeof(message), 0);
  msgrcv(msgid, &message, sizeof(message), 1, 0);
  return atoi(message.mesg_text);
}
int chiedi_stato_T()
{
  message.mesg_type=1;
  message.mesg_text[0]='A';
  msgsnd(msgid_T, &message, sizeof(message), 0);
  msgrcv(msgid, &message, sizeof(message), 1, 0);
  return atoi(message.mesg_text);
}
void set_L(int stato)
{
  message.mesg_type=1;
  message.mesg_text[0]='B';
  msgsnd(msgid_L, &message, sizeof(message), 0);
  message.mesg_text[0]='0'+stato;
  msgsnd(msgid_L, &message, sizeof(message), 0);
}
void premi_T(double tempo) 
{
  message.mesg_type=1;
  message.mesg_text[0]='B';
  msgsnd(msgid_T, &message, sizeof(message), 0);
  sprintf(message.mesg_text, "%f", tempo);
  msgsnd(msgid_T, &message, sizeof(message), 0);
}

/*[0]=0/1->button/switch [1]="comando corrispondente (A, B, C, D) [2]=Pid_Coda*/
int main(int argc, char* argv[])
{
  char pipes_path[MAX_BUFF_SIZE];
  key_t key, key_T, key_L, key_C, key_P;
  key = ftok("/tmp/ipc/mqueues", getpid());   /*si apre la pipe per la comunicazione con il main*/
  msgid = msgget(key, 0666|IPC_CREAT);
 
  key_P = ftok("/tmp/ipc/mqueues", getppid());
  msgid_P = msgget(key_P, 0666|IPC_CREAT);
    
  key_C = ftok("/tmp/ipc/mqueues", atoi(argv[2])); /*si apre la pipe con la Coda*/
  msgid_C = msgget(key_C, 0666|IPC_CREAT);

  /*creo un led e ed una pipe che connetta S<->Led*/
  pid=fork();
  if(pid==0)
  {
    nome_pipe(getpid(), pipes_path);
	mkfifo(pipes_path, 0666);       /*creo una pipe con nome*/
    execlp(path_L, arg_0, NULL);
    exit(0);
  }
  
  pid_L=pid;
  key_L = ftok("/tmp/ipc/mqueues", pid_L);
  msgid_L = msgget(key_L, 0666|IPC_CREAT);
  
  /*creo un T(button/switch) e ed una pipe che connetta S<->T*/
  pid=fork();
  if(pid==0)
  {
    nome_pipe(getpid(), pipes_path);
	mkfifo(pipes_path, 0666);       /*creo una pipe con nome*/

    if(argv[0][0]=='0') /*0->button, 1->switch*/
      execlp(path_B, arg_0, NULL);
    else execlp(path_S, arg_0, NULL);

    exit(0);
  }

  pid_T=pid;
  key_T = ftok("/tmp/ipc/mqueues", pid_T);
  msgid_T = msgget(key_T, 0666|IPC_CREAT);

  while(1)
  {
    char primo;
    msgrcv(msgid, &message, sizeof(message), 1, 0);
    primo=message.mesg_text[0];
    if(primo=='A')
    {

      /*resetta lo stato del led in modo che sia concorde a T*/
      set_L(chiedi_stato_T());
      
      message.mesg_type=1;
      sprintf(message.mesg_text, "%i", chiedi_stato_L());
      msgsnd(msgid_P, &message, sizeof(message), 0);
    }
    
    if(primo=='B')
    {
      /*riceve il tempo di spegnimento (per gli switch è indifferente)*/
      msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);
      if(!chiedi_stato_T())
        aziona_T(argv[1][0]);  /*se T non è in pressione l'azione del comando viene eseguita*/
      
      premi_T(atof(message.mesg_text));
    }

    /*SIGTERM->terminare il processo, comunicare al led, a T e a Coda di terminare*/
    if(primo=='E')
    {
      message.mesg_type=1;
      message.mesg_text[0]='E';
      msgsnd(msgid_C, &message, sizeof(message), 0);
      msgsnd(msgid_L, &message, sizeof(message), 0);
      msgsnd(msgid_T, &message, sizeof(message), 0);
	    
      wait(NULL);
      msgctl(msgid, IPC_RMID, NULL);
      exit(0);
    }

  }

return 0;
}

