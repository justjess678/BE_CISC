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
int cpt = 0;
int tabValCapteur[N];


/**************************fonctions******************************/
int updateList(char* filename, int values[N], double avg)
{
	int i;
	FILE *fptr;
	fptr = fopen(filename,"w+");
	if (fptr == NULL) 
    { 
        printf("Cannot open file %s \n", filename); 
        exit(0); 
    }
	//fills file with new values
	for(i=0 ; i<N ; i++)
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
	int i;
	for(i = 0; i < N-1; i++){
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

double mean3,mean2,mean1 = 0;

	/*mode secondaire (recovery)*/
	while(*mode ==1){

	}


	/*mode primaire*/
	while(1)
	{	 
		printf("thread %lu assure tâche principale \n",pthread_self());
		//Lecture "securisee" de la valeur capteur
		pthread_mutex_lock(&mutexLectureCapteur);
		/*procéduure aléatoire de défaillance en crash*/
		bug = rand()%12;
		if(bug == 1){
			//Le système lève une défaillance en crash et fait 'planter' le thread
			printf("thread %lu faute en crash \n",pthread_self());
			fflush(stdout);
			while(1){}

		} 
		/*incrémentation du wtachdog avec utilisation du mutex associé*/
		pthread_mutex_lock(&mutexWatchDog);
		watchdog++;
		pthread_mutex_unlock(&mutexWatchDog);

		/*gestion de la variable partagée pour la gestion du capetru*/
		while(avail==0)
		{
			pthread_cond_wait(&cond, &mutexLectureCapteur);
		}		
		avail=0;
		pthread_mutex_unlock(&mutexLectureCapteur);
	
		//staockage de la valeur du capteur dans un tableau
		tabValCapteur[cpt] = valCapteur;
		cpt++;
	
		if(cpt>=N) // si on a les 10 valeurs, on calcul la moyenne
		{
			mean1 =  avgCalc(tabValCapteur);
			mean2=mean1;
			printf("Moyenne1 : %lf\n\n", mean1);

			/*procéduure aléatoire de défaillance en valeur*/
			bug = rand()%2;
			if(bug == 0){
				mean2+=1;
			}
			printf("Moyenne2 : %lf\n\n", mean2);

			if (mean1 == mean2){
				//si pas de défaillance en valeur
				updateList(filename, tabValCapteur, mean1);
			}
			else{
				//si 1ere défaillance en valeur on effectue une deuxième fois le test de comparaison
				mean3=mean1;
				bug = rand()%2;
				if(bug == 0){
					mean3-=1;
				}
				printf("Moyenne3 : %lf\n\n", mean3);

				//procédure de vote majoritaire
				if (mean1 == mean3){
					updateList(filename, tabValCapteur, mean1);
				}
				else{
					if (mean2 == mean3){
						updateList(filename, tabValCapteur, mean2);
					}
					else{
						//Le système lève une défaillance en valeur et fait 'planter' le thread
						printf("thread %lu faute en valeur \n",pthread_self());
						fflush(stdout);
						while(1){}
					}
				}
			}
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
	// création du thread chargé de la gestion du capteur
	pthread_create(&capteur,mesAttributNULL, threadCapteur, NULL);
/****************************Code Service Primaire*****************************/	

	int modeThread1=2;
	int modeThread2=1;

	/*création des threads 1 et 2
	le thread n°1 est lancé en mode primaire, 
	tandis que le deux est lancé en mode secondaire*/
	pthread_create(&th1,mesAttributNULL, threadService, &modeThread1);
	pthread_create(&th2,mesAttributNULL, threadService, &modeThread2);

	int oldWatchdog=watchdog;
	int threadActiv=1;
	
	while(1)
	{	
		/*Ce délai sert à cadencer le test du watchdog.
		Il doit être suffisament grand pour laisser le temps au service primaire de se déouler correctement,
		et suffisament petit pour ne pas mettre trop de temps à détecter une faute en crash*/
		sleep(1.6); 

		pthread_mutex_lock(&mutexWatchDog);
		printf("wd %d %d\n", oldWatchdog ,watchdog);

		/*test du watchdog*/
		if(oldWatchdog !=watchdog){
			//Aucun problème détecté
			oldWatchdog=watchdog;
			pthread_mutex_unlock(&mutexWatchDog);
		}
		else{
			/*faute en crash détectée*/
			watchdog=0;
			oldWatchdog=watchdog;
			pthread_mutex_unlock(&mutexWatchDog);

			/*suivant le thread actif:
			on passe le thread secondaire en mode primaire.
			on s'assure de libérer les mutexs nécessaire à l'éxecution du szrvice primaire
			on cancel l'ancien thread primaire
			on le relance en mode secondaire*/
			if(threadActiv==1){
				modeThread2=2;
				pthread_mutex_unlock(&mutexLectureCapteur);
				threadActiv=2;

				pthread_cancel(th1);
				modeThread1=1;
				pthread_create(&th1,mesAttributNULL, threadService, &modeThread1);
			}
			else{
				modeThread1=2;
				pthread_mutex_unlock(&mutexLectureCapteur);
				threadActiv=1;

				pthread_cancel(th2);
				modeThread2=1;
				pthread_create(&th2,mesAttributNULL, threadService, &modeThread2);
			}

		}
	}
	
/******************************************************************************************/
	
	return 0;
}
