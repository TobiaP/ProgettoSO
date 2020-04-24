#include "util.h"

volatile int flag_usr1 = 0;
volatile int flag_usr2 = 0;
volatile int flag_term = 0;

/* handler dei segnali */
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

int pid_L, pid_T, pid, msgid_T, msgid_L, msgid_C;
char pipe_L[MAX_BUFF_SIZE], pipe_T[MAX_BUFF_SIZE];

void azione_T(char azione)
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
  kill(pid_L, SIGUSR1);
  msgrcv(msgid_L, &message, sizeof(message), 1, 0);
  return atoi(message.mesg_text);
}
int chiedi_stato_T()
{
  kill(pid_T, SIGUSR1);
  msgrcv(msgid_T, &message, sizeof(message), 1, 0);
  return atoi(message.mesg_text);
}
void set_L(int stato)
{
  message.mesg_type=1;
  message.mesg_text[0]='0'+stato;
  msgsnd(msgid_L, &message, sizeof(message), 0);
  kill(pid_L, SIGUSR2);
}
void premi_T(double tempo) 
{
  message.mesg_type=1;
  sprintf(message.mesg_text, "%f", tempo);
  msgsnd(msgid_T, &message, sizeof(message), 0);
  kill(pid_T, SIGUSR2);
}

/*[0]=0/1->button/switch [1]="comando corrispondente (A, B, C, D) [2]=Pid_Coda*/
int main(int argc, char* argv[])
{

  key_t key = ftok("/tmp/ipc/mqueues", getpid());   /*si apre la pipe per la comunicazione con il main*/
  int msgid = msgget(key, 0666|IPC_CREAT);
    
  key_t key_C = ftok("/tmp/ipc/mqueues", atoi(argv[2])); /*si apre la pipe con la Coda*/
  msgid_C = msgget(key, 0666|IPC_CREAT);

  /*creo un led e ed una pipe che connetta S<->Led*/
  pid=fork();
  if(pid==0)
  {
    sprintf(pipe_L, "%s%d", path_pipe, getpid());
    mkfifo(path_pipe, 0666);
    execlp(path_L, arg_0, NULL);
    exit(0);
  }
  
  pid_L=pid;
  key_t key_L = ftok("/tmp/ipc/mqueues", pid_L);
  msgid_L = msgget(key_L, 0666|IPC_CREAT);
  
  /*creo un T(button/switch) e ed una pipe che connetta S<->T*/
  pid=fork();
  if(pid==0)
  {
    sprintf(pipe_T, "%s%d", path_pipe, getpid());
    mkfifo(path_pipe, 0666);

    if(argv[0][0]=='0') /*0->button, 1->switch*/
      execlp(path_B, arg_0, NULL);
    else execlp(path_S, arg_0, NULL);

    exit(0);
  }

  pid_T=pid;
  key_t key_T = ftok("/tmp/ipc/mqueues", pid_T);
  msgid_T = msgget(key_T, 0666|IPC_CREAT);

  signal(SIGTERM, sighandler_int);
  signal(SIGUSR1, sighandler_int);
  signal(SIGUSR2, sighandler_int);

  while(1)
  {
    /*SIGUSR1->inviare stato Led*/
    if(flag_usr1)
    {
      flag_usr1=0;

      /*resetta lo stato del led in modo che sia concorde a T*/
      set_L(chiedi_stato_T());
      
      message.mesg_type=1;
      sprintf(message.mesg_text, "%i", chiedi_stato_L());
      msgsnd(msgid, &message, sizeof(message), 0);
    }
    
    /*SIGUSR2->premere T*/
    if(flag_usr2)
    {
      flag_usr2=0;
      /*riceve il tempo di spegnimento (per gli switch è indifferente)*/
      msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);
      if(!chiedi_stato_T())
        aziona_T(argv[1][0]);  /*se T non è in pressione l'azione del comando viene eseguita*/
      
      premi_T(atof(message.mesg_text));
    }

    /*SIGTERM->terminare il processo, comunicare al led, a T e a Coda di terminare*/
    if(flag_term)
    {
      flag_term=0;
      message.mesg_type=1;
      message.mesg_text[0]='E';
      msgsnd(msgid_C, &message, sizeof(message), 0);
          
      kill(pid_L, SIGTERM);
      kill(pid_T, SIGTERM);
      wait(NULL);
      msgctl(msgid, IPC_RMID, NULL);
      exit(0);
    }

  }

return 0;
}

