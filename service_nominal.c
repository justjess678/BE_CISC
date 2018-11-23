#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutexLectureCapteur = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
#define N 10

int valCapteur = 0;
int avail=0;

char * filename = "memory.txt";


/**************************fonctions******************************/
int updateList(char* filename, int values[N], double avg)
{
	FILE *fptr;
	fptr = fopen(filename,"w+");
	if (fptr == NULL) 
    { 
        printf("Cannot open file %s \n", filename); 
        exit(0); 
    }
	//fills file with new values
	for(int i=0 ; i<N ; i++)
	{
		fprintf(fptr,"%d\n",values[i]);
	}
	fprintf(fptr,"%lf\n",avg);
	fclose(fptr);
	return 0;
}

double avgCalc(int values[N])
{
	double sum = 0, average = 0;
	int cnt = 0;
	for(int i = 0; i < N-1; i++){
		if(values[i]>-1){
        	sum += values[i];
			cnt++;
		}
    }
    average = sum / cnt;
	return average;
}



/*******************************threads***************************************/

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
	int cpt = 0;
	int tabValCapteur[N];
	double mean = 0;
	while(1)
	{
		//Lecture "securisee" de la valeur capteur
		pthread_mutex_lock(&mutexLectureCapteur);
		while(avail==0)
		{
		pthread_cond_wait(&cond, &mutexLectureCapteur);
		}		
		printf("Valeur lue: %d\n", valCapteur);
		avail=0;
		pthread_mutex_unlock(&mutexLectureCapteur);
		
		
		tabValCapteur[cpt] = valCapteur;
		cpt++;
		
		if(cpt>=N) // on a les 10 valeurs
		{
			mean =  avgCalc(tabValCapteur);
			printf("\nValeur lue: %lf\n", mean);
			updateList(filename, tabValCapteur, mean);
			cpt = 0;
		}
		
	
	}
}




/************************************main**************************************/
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
