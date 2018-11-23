#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t mutexLectureCapteur = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexWatchDog = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
#define N 10

int valCapteur = 0;
int avail=0;

char * filename = "memory.txt";

int watchdog;


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



void * threadService (void * arg){

int bug;
int * mode = (int *) arg;

int cpt = 0;
int tabValCapteur[N];
double mean = 0;

	while(*mode ==1){

	}



	while(1)
	{	 
		//Lecture "securisee" de la valeur capteur
		pthread_mutex_lock(&mutexLectureCapteur);
		bug = rand()%8;
		if(bug == 1){
			printf("thread %lu bug \n",pthread_self());
			fflush(stdout);
			while(1){}

		} 

		pthread_mutex_lock(&mutexWatchDog);
		watchdog++;
		pthread_mutex_unlock(&mutexWatchDog);

		while(avail==0)
		{
			pthread_cond_wait(&cond, &mutexLectureCapteur);
		}		
		avail=0;
		pthread_mutex_unlock(&mutexLectureCapteur);
	
	
		tabValCapteur[cpt] = valCapteur;
		cpt++;
	
		if(cpt>=N) // on a les 10 valeurs
		{
			mean =  avgCalc(tabValCapteur);
			printf("Moyenne: %lf\n\n", mean);
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
	pthread_t th1,th2;
	

/****************************Code Capteur************************************/	
	pthread_create(&capteur,mesAttributNULL, threadCapteur, NULL);
/****************************Code Service Primaire*****************************/	

	int modeThread1=2;
	int modeThread2=1;

	pthread_create(&th1,mesAttributNULL, threadService, &modeThread1);
	pthread_create(&th2,mesAttributNULL, threadService, &modeThread2);

	int oldWatchdog=watchdog;
	int threadActiv=1;
	
	while(1)
	{	
		sleep(2); 

		pthread_mutex_lock(&mutexWatchDog);
		printf("wd %d %d\n", oldWatchdog ,watchdog);

		if(oldWatchdog !=watchdog){
			oldWatchdog=watchdog;
			pthread_mutex_unlock(&mutexWatchDog);
		}
		else{
			watchdog=0;
			oldWatchdog=watchdog;
			pthread_mutex_unlock(&mutexWatchDog);

			if(threadActiv==1){
				modeThread2=2;
				pthread_mutex_unlock(&mutexLectureCapteur);
				threadActiv=2;

				pthread_cancel(th1);
				modeThread1=1;
				//sleep(0.2); 
				pthread_create(&th1,mesAttributNULL, threadService, &modeThread1);
			}
			else{
				modeThread1=2;
				pthread_mutex_unlock(&mutexLectureCapteur);
				threadActiv=1;

				pthread_cancel(th2);
				modeThread2=1;
				//sleep(0.2); 
				pthread_create(&th2,mesAttributNULL, threadService, &modeThread2);
			}

		}
	}
	
/******************************************************************************************/
	
	return 0;
}
