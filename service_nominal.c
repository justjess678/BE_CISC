#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutexLectureCapteur = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int valCapteur = 0;

int avail=0;

void * threadCapteur(void * arg)
{

	while(1)
	{
		pthread_mutex_lock(&mutexLectureCapteur);
		valCapteur = rand()%20; 
		avail=1;
		pthread_mutex_unlock(&mutexLectureCapteur);
		pthread_cond_signal(&cond);
		sleep(1);     
		
	}
}


void * threadServicePrimaire (void * watchdog)
{
	while(1)
	{
		pthread_mutex_lock(&mutexLectureCapteur);
		while(avail==0)
		{
		pthread_cond_wait(&cond, &mutexLectureCapteur);
		}		
		printf("\nValeur lue: %d\n", valCapteur);
		avail=0;
		pthread_mutex_unlock(&mutexLectureCapteur);
	
	}
}


int main()
{
	srand(time(NULL));

/****************************Declaration Partie Capteur************************************/
	pthread_t capteur;
	pthread_attr_t *mesAttributNULL = NULL;
/****************************Declaration Service Primaire**********************************/
	pthread_t sevicePrim;
	

/****************************Code Capteur************************************/	
	pthread_create(&capteur,mesAttributNULL, threadCapteur, NULL);
/****************************Code Service Primaire*****************************/	
	pthread_create(&sevicePrim,mesAttributNULL, threadServicePrimaire, NULL);
	
	while(1)
	{
	}
	
/******************************************************************************************/
	
	return 0;
}
