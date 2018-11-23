#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutexLectureCapteur = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


int avail=0;

void * threadCapteur(void * valCapteur)
{
	int * a = (int *)valCapteur; 
	while(1)
	{
		pthread_mutex_lock(&mutexLectureCapteur);
		*a = rand()%20; 
		avail=1;
		pthread_mutex_unlock(&mutexLectureCapteur);
		pthread_cond_signal(&cond);
		sleep(1);     
		
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
		pthread_mutex_lock(&mutexLectureCapteur);
		while(avail==0)
		{
		pthread_cond_wait(&cond, &mutexLectureCapteur);
		}		
		printf("\nValeur lue: %d\n", *valCapteur);
		avail=0;
		pthread_mutex_unlock(&mutexLectureCapteur);
	
	}
/******************************************************************************************/
	
	return 0;
}
