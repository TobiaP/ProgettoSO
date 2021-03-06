#include "util.h"

int N_proc = 1;                     /*numero di processi attuale >= 1*/
int nuovo_id=0;  /*ultima identità creata*/
int size_Coda=0; /*numero messaggi in queue*/
int pipeF[2][2];                    /*vengono create due pipe con cui comunicare ai figli in modo che si possa cancellare */
                                    /*un figlio e nel frattempo creare l'altro*/
int selezionato=0;   /*processo selezionato da C (default 0), se si preme un altro T torna a 0*/
int pipeatt=0;
pid_t primo_pid;
int msgid, msgid_main;				/*nome pipe con comandi, nome pipe con main*/
int coda[MAX_QUEUE]; /*tiene memorizzati tutti gli identificativi dei processi*/
int i;

int new_identity() /*crea una nuova identità (nuovo_id+1)*/
{
  nuovo_id++;
  return nuovo_id;
}

void crea_primo_figlio(int fident)
{
  pid_t pid_n;
  char fid[MAX_BUFF_SIZE];

  pipe(pipeF[pipeatt]);
  pid_n=fork();
  if(pid_n==0)/*creazione processo figlio*/
  {
    close(pipeF[pipeatt][1]);    /*il figlio è il lettore della pipe*/
    dup2(pipeF[pipeatt][0], 0);  /*ora il figlio legge dalla pipe con scanf()*/
    close(pipeF[pipeatt][0]);    /*si chiude la lettura originale*/
    
    sprintf(fid, "%i", fident);
    execlp(path_P, fid, NULL);
    exit(0);
  }else{
    close(pipeF[pipeatt][0]);    /*questo processo deve inviare sulla pipe*/
    dup2(pipeF[pipeatt][1], 1);  /*questo processo permette di inviare con printf()*/
    close(pipeF[pipeatt][1]);    /*si chiude la scrittura originale*/
    primo_pid = pid_n;
  }
  size_Coda=1;
}


void invia_messaggio(char* mess)
{
  message.mesg_type=1;
  strcpy(message.mesg_text, mess);
  msgsnd(msgid_main, &message, sizeof(message), 1);
}

void inserisci_figlio(int nident) /*inserisce un figlio in fondo alla coda*/
{
  char nid[MAX_BUFF_SIZE]; 
  
  printf("B");                          /*scrivo al figlio che voglio aggiungere un Processo*/                           
  sprintf(nid, "%i", nident); 
  printf("%s", nid);                    /*scrivo al figlio l'identità del nuovo processo*/
      
}


int main(int argc, char* argv[])
{
  key_t key_m, key;
  crea_primo_figlio(new_identity());
  
  key_m = ftok("/tmp/ipc/mqueues", getppid());
  msgid_main = msgget(key_m, 0666|IPC_CREAT); 		/*si apre la pipe con il main*/

  key = ftok("/tmp/ipc/mqueues", getpid());   /*si apre la pipe per la comunicazione con i comandi*/
  msgid = msgget(key, 0666|IPC_CREAT);


  while(1)
  {
    int no_msg = msgrcv(msgid, &message, sizeof(message), 1, IPC_NOWAIT);
    if(no_msg!=-1)
    {
      switch(message.mesg_text[0])
      {
        case 'A':   /*rimozione del primo figlio*/
        {
          selezionato=0;
          if(size_Coda>1)
          {
            pid_t pid_n;
            size_Coda--; 
            for(i=0; i<MAX_QUEUE; i++)  /*si aggiorna la coda eliminando il primo identificativo*/
            {
              if(i>=size_Coda)
                coda[i]=0;
              else
                coda[i]=coda[i+1];
            }

            if(pipeatt==1)
              pipe(pipeF[0]);
            else pipe(pipeF[1]);
            pid_n=fork();
            if(pid_n==0)/*creazione processo figlio (ora in modo che non rimanga un P attivo)*/
            {
              char fid[MAX_BUFF_SIZE];
              if(pipeatt==1)  /*si utilizza la pipe che non è in uso dal figlio in eliminazione*/
              {
                close(pipeF[0][1]);
                dup2(pipeF[0][0], 0);  /*ora il figlio legge dalla pipe con scanf()*/
                close(pipeF[0][1]);
              }else{
                close(pipeF[1][1]);
                dup2(pipeF[1][0], 0);  /*ora il figlio legge dalla pipe con scanf()*/
                close(pipeF[1][1]);
              }
              sprintf(fid, "%i", coda[0]);
              execlp(path_P, fid, NULL);
              exit(0);
            }

            printf("%c", 'E');    /*si comunica al primo processo che tutti vanno eliminati*/
            size_Coda=1;

            if(pipeatt==0)        /*si cambia la pipe anna attuale*/
              pipeatt=1;
            else pipeatt=0;

            close(pipeF[pipeatt][0]);    /*questo processo deve inviare sulla pipe*/
            dup2(pipeF[pipeatt][1], 1);  /*questo processo permette di inviare con printf()*/
            close(pipeF[pipeatt][1]);    /*si chiude la scrittura originale*/
              
            primo_pid = pid_n;

            for(i=0; i<size_Coda; i++)
            {
              inserisci_figlio(coda[i]);
            }
            
            invia_messaggio("Il nodo è stato eliminato con successo\n");
          }else
            invia_messaggio("non ci sono abbastanza elementi nella coda\n");
        }break;

        case 'B':   /*aggiunta di un nuovo figlio in fondo*/
        {
          selezionato=0;
          if(size_Coda<MAX_QUEUE)
          {
            inserisci_figlio(new_identity());
            size_Coda++;                         /*la coda ha un elemento in più */
            coda[size_Coda]=nuovo_id;             /*aggiungo l'ultimo elemento in coda*/
            invia_messaggio("il nodo è stato aggiunto con successo\n"); /*viene inviato al main un messaggio di successo*/
          }else
            invia_messaggio("limite processi in Coda raggiunto\n"); /*altrimenti viene inviato un messaggio di errore al main*/
        }break;

        case 'C':     /*selezionare un elemento e invia la lista*/
        {             /*con l'elemento selezionato */
          char lung[MAX_BUFF_SIZE];  
          selezionato++;                 /*l'elemento selezionato aumenta di uno ciclicamente*/
          if(selezionato>=size_Coda)
            selezionato = 0;
                           
          sprintf(lung, "%i", size_Coda);
          invia_messaggio(lung);          /*invio la lunghezza della coda*/
          for(i=0; i<size_Coda; i++)
          {
            char mess[MAX_BUFF_SIZE];
            if(i!=selezionato)
              sprintf(mess, "%i ", coda[i]);        
            else sprintf(mess, "(%i) ", coda[i]);
            invia_messaggio(mess);         /*invio gli elementi*/
          }
        }break;

        case'D':
        {
          if(size_Coda>1)
          {
          char sel[MAX_BUFF_SIZE];
          sprintf(sel, "%i", selezionato);
          printf("%c", 'E');      /*comunico che dall'elemento selezionato in poi vanno eliminati tutti*/
          printf("%s", sel);

          size_Coda=selezionato;  
          for(i=selezionato; i<MAX_QUEUE; i++)  /*aggiorno la coda togliendo il */
          {
            if(i>=size_Coda)
              coda[i]=0;
            else coda[i]=coda[i+1];
          }
          size_Coda--;
          for(i=selezionato; i<MAX_QUEUE; i++)
          {
            if(i<size_Coda)
              coda[i]=coda[i+1];
            else coda[i]=0;
          }

          for(i=selezionato; i<size_Coda; i++)
            inserisci_figlio(coda[i]);
          selezionato=0;

          invia_messaggio("Il figlio è stato eliminato con successo");
          }else invia_messaggio("non ci sono abbastanza messaggi in coda");
        }break;

        case 'E': /*il processo termina tutti i figli e poi termina*/
        {
          msgctl(msgid, IPC_RMID, NULL);
          printf("%c", 'E');
          wait(NULL);
          exit(0);
        }
      }

    }
  }
  

  return 0;
}



