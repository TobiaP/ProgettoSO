#include "util.h"

struct mesg_buffer { 
    long mesg_type; 
    char mesg_text[100]; 
} message; 

#define path_pipe "/tmp/ipc/"
#define path_L "bin/Led.c"
#define path_B "bin/Button.c"
#define path_S "bin/Switc.c"
#define path_P "bin/Processo.c"
#define arg_0 "1"
#define MAX_BUFF_SIZE 1024


pid_t pid, pid_figlio, pid_padre;
int pipeF[2], pipeP[2], fig=0;/*pipe con il figlio, pipe con il padre, 1->c'è figlio / 0->non c'è*/

unsigned long long int ident, fident;

void crea_figlio(char* nident)
{
  fident=atoll(nident);

  pipe(pipeF);
  pid_t pid_n;
  pid_n=fork();
  if(pid_n==0)/*creazione processo figlio*/
  {
    close(pipeF[1]);    /*il figlio è il lettore della pipe*/
    dup2(pipeF[0], 0);  /*ora il figlio legge dalla pipe con scanf()*/
    close(pipeF[0]);    /*si chiude la lettura originale*/
    
    execlp(path_P, nident);
    exit(0);
  }else{
    close(pipeF[0]);    /*questo processo deve inviare sulla pipe*/
    dup2(pipeF[1], 1);  /*questo processo invia con printf()*/
    close(pipeF[1]);    /*si chiude la scrittura originale*/

  }
}

/*[0]=identificatore */
int main(int argc, char* argv[])
{
  
  pid=getpid();
  pid_padre=getppid();
  ident=atoll(argv[0]);

  while(1)
  {
    char lettera;
    char *lettura;
    scanf("%c", &lettera);
    
    switch(lettera)
    {
    case 'A':                   /*eliminazione da un processo in poi*/
    {
      scanf("%s", lettura);
      if(atoll(lettura)==fident)
      {
        printf("%s", "E\n");    /*se il suo figlio è il processo da eliminare*/
        wait(NULL);             /*glielo comunica altrimenti continua cosi*/
      }else
        printf("%c%s", lettera, lettura);
    }break;

    case 'B':                   /*aggiunta figlio*/
    {
      scanf("%s", lettura);
      if(fig)                   /*se il figlio esiste passa il nuovo id*/
      {
        printf("%c", 'B');      /*e la lettera B*/
        printf("%s", lettura);  /*a quello successivo*/
      }
      else                      /*altrimenti crea un figlio e gli */
        crea_figlio(lettura);   /*passa il nuovo identificativo*/
    }break;

    case 'C':                   /*elimina tutti gli elementi da un figlio in poi*/
    {
      scanf("%s", lettura);     
      if(fident==atoll(lettura))  /*se il prossimo è l'elemento da eliminare glie lo comunico*/
      {
        printf("%c", 'E');
        pid_figlio=1;
        fident=0;
        fig=0;
      }
      else{                        /*altrimenti gli passo il messaggio*/
        printf("%c", 'C');
        printf("%s", lettura);
      }

    }

    case 'E':                   /*eliminazione di tutti i processi*/
    {                           /*da questo in poi*/
      if(fig)
      {                   
        printf("%c", 'E');      /*comunica all'elemento successivo di fare lo stesso*/
        wait(NULL);             
      }
      exit(0);                  /*quando il figlio (se c'è) si è eliminato lo fa anche questo*/
    }break;


    }
  }

  return 0;
}

