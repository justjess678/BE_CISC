#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//globals
int REBOOT=1;
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
	for(int i=0;i<10;i++){
		fprintf(fptr,"%d\n",values[i]);
	}
	fclose(fptr);
	return 0;
}

double operation(int values[N]){
	double sum = 0, average = 0;
	for(int i = 0; i < N-1; i++){
        sum += values[i];
    }
    average = sum / (N-1);
	return average;
}

int main(){
	//initialisation
	int values[N-1];
	for(int i=0;i<N;i++){
		values[i]=0;
	}
	double avg=0;

	//memory file handling
	FILE *fp;
	fp = fopen(filename,"r");
	printf("File opened\n");		
	if(REBOOT==1 && fp != NULL){
		printf("Reboot\n");
		fclose(fp);
		printf("File closed\n");
		//get values from file
		fp = fopen(filename,"a+");
		for(int i=(N-1);i>=0;i--){
			fscanf(fp, "%d\n", &values[(N-1)-i]);
		}
		printf("Old values:\n");
		for(int i=0;i<N;i++){
			printf("\t%d:\t%d\n",i,values[i]);
		}
		fclose(fp);
		printf("Old memory values recovered\n");
	}else{
		printf("Memory file doesn't exist or is not a reboot\n");
		if(fp != NULL){
			fclose(fp);
			printf("Memory File closed\n");
			//delete previous file and make a new one
			if(remove(filename)!=0){
				printf("Error deleting previous memory file %s \n", filename); 
				exit(0);
			}
			printf("Old memory file deleted\n");
		}
	}

	printf("Loop started\n");
	/*start running loop*/

	/*receive avg here*/
	printf("Receiving started\n");

	/*calculate average of values*/
	avg = operation(values);
	
	printf("New values:\n");
	for(int i=0;i<N;i++){
		printf("\t%d:\t%d\n",i,values[i]);
	}

	/*checkpoint*/
	updateList(filename, values, avg);

	/*Send OK to watchdog*/
	
}

