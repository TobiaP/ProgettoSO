#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

int fdp, fdl[2], fdbs[2]; //file descriptor della pipe con padre, led, button/switc


void *led()
{
	
	int l=0;
	while(1)
	{
		int val;
		
		read(fdl[0], &val, 1);
		switch(val)		//0->accendi
		{				//1->spegni
			case 0:		//2->invia stato
				l=1;
		break;
			case 1:
				l=0;
		break;
			case 2:
			{
				val=l;
				write(fdl[1], &val, 1);
			}	
		}
		
	}
	
}

void *switc()
{
	int active=0;
	while(1)
	{
		int val;
		read(fdbs[0], &val, 1);
		
		switch(val)		//0->premi
		{				//2->invia stato
			case 0:		
			{
				if(active)
					active=0;
				else active=1;
			}
		break;
			case 2:
				{
				val=active;
				write(fdbs[1], &val, 1);
				}
		}
	}
}

void *button()
{
	int active=0;
	double tempo=0.5, tempoatt; //tempo standard(minimo), tempo reale
	clock_t inizio, fine;
	while(1)
	{
		int val;
		unsigned int time;
		read(fdbs[0], &val, 1);
		if(active && ((double)inizio-fine)/CLOCKS_PER_SEC>=tempoatt)
			active=0;	//se il led è acceso ed è scaduto il tempo lo spengo
		
		switch(val)				//0->ON
		{						//1->ON con tempo diverso					
			case 0:				//2->invia stato
			{
				if(active==0)
				{
					tempoatt=time;
					active=1;
				}
			}
		break;
			case 1:
			{
				if(active==0)
				{
					int t1;
					active=1;
					read(fdbs[0], &t1, 1);
					if((double)t1/1000>=tempo)
						tempoatt=(double)t1/1000;
					else tempoatt=tempo;
				}
			}
		break;
			case 2:
			{
				val=active;
				write(fdbs[1], &val, 1);
			}	
		}
	}
}

//argv = [button=1/switc=0, pipe_con_padre]
int main(int argc, char* argv[])    
{
	
	//viene aperta in RDWR la pipe con il main
	//pipep = argv[1];
	//fdp = open(pipep, O_RDWR);
	
	pthread_t l, t;
	
	//vengono creati i thread dei led e dei bottoni/switc
	phtread_create(&l, NULL, led, NULL);
	
	if(argv[0])
		phtread_create(&t, NULL, button, NULL);
	else
		phtread_create(&t, NULL, switc, NULL);
		
	while(1)
	{
		int val, v1;
		scanf("%d", val);
		
		switch(val)		//0->premi
		{				//1->premi con tempo
			case 0:		//2->richiedi stato led
				{
					v1=0;
					write(fdbs[1], &v1, 1);	//dico che va premuto t
					v1=2;	//richiedo lo stato di t
					write(fdbs[1], &v1, 1);
					read(fdbs[0], &v1, 1);
					
					write(fdl[1], &v1, 1); //innvio lo stato di t al led
				}
		break;
			case 1:	//se t è uno switc non cambia niente ma viene letto un tempo
				{
					int t1;
					scanf("%d", t1);
					v1=0;
					if(argv[0])
					{
						write(fdbs[1], &t1, 1);//se è un bottone invio il tempo
						v1=1;
						write(fdbs[1], &v1, 1);
					}else write(fdbs[1], &v1, 1); //altrimenti viene solo premuto
					v1=2;	//richiedo lo stato di t
					write(fdbs[1], &v1, 1);
					read(fdbs[0], &v1, 1);
					
					write(fdl[1], &v1, 1); //innvio lo stato di t al led
				}
		break;
			case 2:
				{
					v1=2;	//richiedo lo stato di t
					write(fdbs[1], &v1, 1);
					read(fdbs[0], &v1, 1);
					write(fdl[1], &v1, 1); //innvio lo stato di t al led
					
					v1=2;
					write(fdl[1], &v1, 1);
					read(fdl[0], &v1, 1);
					printf("%d", v1);
				}
		}
	}
	
	
	//il main aspetta che i 2 thread terminino prima di terminare
	pthread_join(l, NULL);
	pthread_join(t, NULL);
}
