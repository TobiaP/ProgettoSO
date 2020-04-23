#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

//#include "util.h"
#include "s.h"

char type;
pid_t pid, pid_SA, pid_SB, pid_SC, pid_SD, pid_Coda;
int msgid, msgid_SA, msgid_SB, msgid_SC, msgid_SD, msgid_Coda;

// variabili per stampa
int led_status[] = {0, 0, 0, 0};
int pint_needed = 0;

void init(); // scelta bott. / interr. ed inizializza S1, S2, S3, S4
void core_buttons();
void core_switch();
void led_check();

int richiesta_stato_led_SA();
int richiesta_stato_led_SB();
int richiesta_stato_led_SC();
int richiesta_stato_led_SD();
void pressione_T_SA(double tempo);
void pressione_T_SB(double tempo);
void pressione_T_SC(double tempo);
void pressione_T_SD(double tempo);
void print();

//-----------------------------------------------------------------------------------------------------------------------
int main() {
	
	init();
	
	// core
	printf("\n\n");
	printf("- 'a' --> delete process \n");
	printf("- 'b' --> create process \n");
	printf("- 'c' --> select next process \n");
	printf("- 'd' --> delete selected process \n");
	printf("- 'E' --> exit \n\n");
	printf("\n[A][B][C][D]\n");
	printf("( )( )( )( )\n\n\n");
	
	if (type == 'b') {
		core_buttons();
	} else {
		core_switch();
	}
	
	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------

// inizializzazione processi
void init() {
	int i = 1;
	
	// Inizializzazione coda
	pid_Coda = fork();
	if (pid_Coda == 0) {
		mkfifo(path_pipe, 0666);
		execlp(path_C, NULL);	
		exit(0);
	}
	key_y key_Coda = ftok("/tmp/ipc/mqueues", pid_Coda);
	msgid_Coda = msgget(key_Coda, 0666|IPC_CREAT);
	
	char* pid_C;
	sprintf(pid_C, "%i", pid_Coda);
	
	// scelta bottoni / interruttori
	do {
		printf("Choose buttons or switches: [b/s]\n");
		scanf(" %c", &type);
		if (type != 'b' && type != 's') {
			printf("\nScegliere 'b' o 's'!\n");
		} else {
			i = 0;
		}
	} while (i > 0);
	
	// inizializzazione bottoni / interruttori
	
	// A
	pid = fork();
	if (pid == 0) {                    //codice figlio
		mkfifo(path_pipe, 0666);       //creo una pipe con nome
		if (type == 'b') {
			execlp(path_S, "0", "A", pid_C, NULL);
		} else {
			execlp(path_S, "1", "A", pid_C, NULL);
		}
		exit(0)
	}
	key_t key_SA = ftok("/tmp/ipc/mqueues", pid_SA);	//accedo alla pipe creata nel figlio
	msgid_SA = msgget(key_SA, 0666|IPC_CREAT);
	
	// B
	pid = fork();
	if (pid == 0) {
		mkfifo(path_pipe, 0666);
		if (type == 'b') {
			execlp(path_S, "0", "B", pid_C, NULL);
		} else {
			execlp(path_S, "1", "B", pid_C, NULL);
		}
		exit(0)
	}
	key_t key_SB = ftok("/tmp/ipc/mqueues", pid_SB);
	msgid_SB = msgget(key_SB, 0666|IPC_CREAT);
	
	// C
	pid = fork();
	if (pid == 0) {
		mkfifo(path_pipe, 0666);
		if (type == 'b') {
			execlp(path_S, "0", "C", pid_C, NULL);
		} else {
			execlp(path_S, "1", "C", pid_C, NULL);
		}
		exit(0)
	}
	key_t key_SC = ftok("/tmp/ipc/mqueues", pid_SC);
	msgid_SC = msgget(key_SC, 0666|IPC_CREAT);
	
	// D
	pid = fork();
	if (pid == 0) {
		mkfifo(path_pipe, 0666);
		if (type == 'b') {
			execlp(path_S, "0", "D", pid_C, NULL);
		} else {
			execlp(path_S, "1", "D", pid_C, NULL);
		}
		exit(0)
	}
	key_t key_SD = ftok("/tmp/ipc/mqueues", pid_SD);
	msgid_SD = msgget(key_SD, 0666|IPC_CREAT);
}

void core_buttons() {
	char input;
	double tempo;
	
	printf("\nInput Format: <TastoScelto> <TempoDiPressione>\n");
	
	do {
		printf("\n > ");
		scanf("%c%lf", &input, &tempo);
		
		switch (input) {
			case 'a':
				pressione_T_SA(tempo); // call(S1);
				break;
			case 'b':
				pressione_T_SB(tempo); // call(S2);
				break;
			case 'c':
				pressione_T_SC(tempo); // call(S3);
				break;
			case 'd':
				pressione_T_SD(tempo); // call(S4);
				break;
			case 'E':
				kill(pid_SA, SIGTERM);
				kill(pid_SB, SIGTERM);
				kill(pid_SC, SIGTERM);
				kill(pid_SD, SIGTERM);
				break;
			default:
				printf("ERROR, invalid input!\n");
				break;
		};
		
		// stampa
		led_check();
		if (pint_needed != 0) {
			print();
		}
		
	} while (input != 'E');
}

void core_switch() {
	char input;
	double tempo = 0.0;
	
	printf("\nInput Format: <TastoScelto>\n");
	
	do {
		printf("\n > ");
		scanf(" %c", &input);
		
		switch (input) {
			case 'a':
				pressione_T_SA(tempo); // call(S1);
				break;
			case 'b':
				pressione_T_SB(tempo); // call(S2);
				break;
			case 'c':
				pressione_T_SC(tempo); // call(S3);
				break;
			case 'd':
				pressione_T_SD(tempo); // call(S4);
				break;
			case 'E':
				kill(pid_SA, SIGTERM);
				kill(pid_SB, SIGTERM);
				kill(pid_SC, SIGTERM);
				kill(pid_SD, SIGTERM);
				break;
			default:
				printf("ERROR, invalid input!\n");
				break;
		};
		
		// stampa
		led_check();
		if (pint_needed != 0) {
			print();
		}
		
	} while (input != 'E');
}

// Verifica Led
int richiesta_stato_led_SA() {
	kill(pid_SA, SIGUSR1);                                  //comunico ad SA la necessità di conoscere lo stato del led
	msgrcv(msgid_SA, &message, sizeof(message), 1, 0);      //leggo lo stato del led
	return atoi(message.mesg_text);                         //converto lo stato del led in int (0, 1)
}

int richiesta_stato_led_SB() {
	kill(pid_SB, SIGUSR1);
	msgrcv(msgid_SB, &message, sizeof(message), 1, 0);
	return atoi(message.mesg_text);
}

int richiesta_stato_led_SC() {
	kill(pid_SC, SIGUSR1);
	msgrcv(msgid_SC, &message, sizeof(message), 1, 0);
	return atoi(message.mesg_text);
}

int richiesta_stato_led_SD() {
	kill(pid_SD, SIGUSR1);
	msgrcv(msgid_SD, &message, sizeof(message), 1, 0);
	return atoi(message.mesg_text);
}

void led_check() {
	// controllo lo stato attuale dei 4 led
	int L1 = richiesta_stato_led_SA();
	int L2 = richiesta_stato_led_SB();
	int L3 = richiesta_stato_led_SC();
	int L4 = richiesta_stato_led_SD();
	
	// li confronto con gli ultimi stampati
	if (L1 != led_status[0]) {
		led_status[0] = L1;
		pint_needed = 1;
	}
	
	if (L2 != led_status[1]) {
		led_status[1] = L2;
		pint_needed = 1;
	}
	
	if (L3 != led_status[2]) {
		led_status[2] = L3;
		pint_needed = 1;
	}
	
	if (L4 != led_status[3]) {
		led_status[3] = L4;
		pint_needed = 1;
	}
}

// Pressione Tasto
void pressione_T_SA(double tempo) {                         //se l'utente non inserisce il tempo deve essere tempo=0.5
	message.mesg_type=1;                                    //creo un messaggio da mandare a SA
	sprintf(message.mesg_text, "%f", tempo);                //in cui inserisco un tempo (nel caso dei bottoni sarà sempre 0.0)
	msgsnd(msgid_SA, &message, sizeof(message, 0));          //lo invio
	kill(pid_SA, SIGUSR2);                                  //comunico a SA che c'è un messaggio da leggere
}

void pressione_T_SB(double tempo) {
	message.mesg_type=1;
	sprintf(message.mesg_text, "%f", tempo);
	msgsnd(msgid_SB, &message, sizeof(message, 0));
	kill(pid_SB, SIGUSR2);
}

void pressione_T_SC(double tempo) {
	message.mesg_type=1;
	sprintf(message.mesg_text, "%f", tempo);
	msgsnd(msgid_SC, &message, sizeof(message, 0));
	kill(pid_SC, SIGUSR2);
}

void pressione_T_SD(double tempo) {
	message.mesg_type=1;
	sprintf(message.mesg_text, "%f", tempo);
	msgsnd(msgid_SD, &message, sizeof(message, 0));
	kill(pid_SD, SIGUSR2);
}

void print() {
	printf("\n\n[A][B][C][D]\n");
	printf("(%d",led_status[0],")(%d",led_status[1],")(%d",led_status[2],")(%d",led_status[3],")\n\n\n");
	pint_needed = 0;
}

