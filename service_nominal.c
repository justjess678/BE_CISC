#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void * threadCapteur(void * valCapteur)
{
	int * a = (int *)valCapteur; //valeur intermédiaire pour pointer sur l'entier
	while(1)
	{
		*a = rand()%20; //Valeur de 0 a 20
		sleep(1);      // Attends une seconde
	}
}



int main()
{
	srand(time(NULL));

/****************************Declaratiion Partie Capteur************************************/
	pthread_t capteur;
	pthread_attr_t *mesAttribut1 = NULL;
	int * valCapteur = (int *)malloc(sizeof(int));
/******************************************************************************/
	

/****************************Code Capteur************************************/	
	pthread_create(&capteur,mesAttribut1, threadCapteur, valCapteur);

	while(1)
	{
		
		printf("\nValeur lue: %d\n", *valCapteur);
		sleep(1);
	}
/******************************************************************************************/
	
	return 0;
}

