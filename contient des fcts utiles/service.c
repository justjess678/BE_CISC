#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//globals
int REBOOT=0;
int N=10;
char* filename = "memory.txt";

void * threadService(void * valCapteur){
	int values[N-1];
	int count=0;
	double avg=0;
	//while(count<N){
	/*wait for sensor value*/
	//
	/*add value to array*/
	//values[count%(N-1)] = valCapteur;
	//count++;
	//}
	/*when array is full (count = N?)*/
	/*calculate average*/
	//avg = operation(values);
	/*save values to file*/
	/*save average to file*/
	//updateList(filename, values, avg);
}

int updateList(char* filename, int values[N], double avg){
	FILE *fptr;
	fptr = fopen(filename,"w+");
	if (fptr == NULL) 
    { 
        printf("Cannot open file %s \n", filename); 
        exit(0); 
    }
	//fills file with new values
	for(int i=0;i<N;i++){
		fprintf(fptr,"%d\n",values[i]);
	}
	fprintf(fptr,"%lf\n",avg);
	fclose(fptr);
	return 0;
}

double operation(int values[N]){
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

int fileSetup(int reboot, char* filename){
	//memory file handling
	FILE *fp;
	fp = fopen(filename,"r");
	printf("File opened\n");		
	if(reboot==1 && fp != NULL){
		printf("Reboot\n");
		fclose(fp);
		printf("File closed\n");
		return 1;
	}else{
		printf("Memory file doesn't exist or is not a reboot\n");
		if(fp != NULL){
			fclose(fp);
			//delete previous file and make a new one
			if(remove(filename)!=0){
				printf("Error deleting previous memory file %s \n", filename); 
				exit(0);
			}
			printf("Old memory file deleted\n");
		}
		return 0;
	}
}

int main(){
	//initialisation
	int values[N];
	for(int i=0;i<N;i++){
		values[i]=-1;
	}
	double avg=-1;

	//memory file handling
	FILE *fp;
	if(fileSetup(REBOOT, filename)==1){
		//get values from file
		fp = fopen(filename,"a+");
		for(int i=(N-1);i>=0;i--){
			fscanf(fp, "%d\n", &values[(N-1)-i]);
		}
		fscanf(fp, "%lf\n", &avg);
		printf("Old values:\n");
		for(int i=0;i<N;i++){
			printf("\t%d:\t%d\n",i,values[i]);
		}
		printf("\tAverage:\t%lf\n",avg);
		fclose(fp);
		printf("Old memory values recovered\n");
	}

	/*receive avg here*/
	printf("Receiving started\n");
	/*int numValsRecieved = 0;
	while(!STOP){
		val = getVal();
		values[numValsRecieved%N] = val;
		numValsRecieved++;
		//calculate average of values
		avg = operation(values);
		printf("\tAverage:\t%lf\n",avg);
		if(numValsRecieved%N == 0){
			//checkpoint
			updateList(filename, values, avg);
			//send OK to watchdog
		}
	}
	*/

	/*calculate average of values*/
	avg = operation(values);
	
	printf("Values:\n");
	for(int i=0;i<N;i++){
		printf("\t%d:\t%d\n",i,values[i]);
	}
	printf("\tAverage:\t%lf\n",avg);

	/*checkpoint*/
	updateList(filename, values, avg);

	/*Send OK to watchdog*/
	
}

