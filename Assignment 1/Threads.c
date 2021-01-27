/*
Name: Swapnali Wakchaure
ID: 700703307
Assignment	1 : C Four Threads Assignment
*/
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

struct thread_data {
	int x1, x2;
};

void *runner(void *params)
{
	struct thread_data *data = (struct thread_data *) params;
	int i, sum = 0;
	for (i=data->x1; i<=data->x2; i++)
	sum = sum + i;

	printf("The sum from %d to %d = %d\n", data ->x1, data ->x2, sum);
	pthread_exit((void *)(intptr_t) sum);
}


#define NUM_THREADS 3c
int main(int argc, char *argv[])
{
	struct thread_data params[NUM_THREADS];
	params[0].x1 = 1; params[0].x2 = 10;
	params[1].x1 = 11; params[1].x2 = 20;
	params[2].x1 = 21; params[2].x2 = 30;
	params[3].x1 = 31; params[3].x2 = 40;

	int i;
	pthread_t tid[NUM_THREADS];
	for (i=0; i<NUM_THREADS; i++) {
	int ret = pthread_create(&tid[i], NULL, runner, (void *)&params[i]);

	if (ret){
		printf ("Error: unable to create thread, %d\n",	ret) ;
		return -1;
		}
	}
	// wait till all threads has completed
	void *status;
	int tot_thread =0 ;
	int out_thread[NUM_THREADS];

	for (i=0; i<NUM_THREADS; i++){
		pthread_join(tid[i], (void**)&status);
		out_thread[i] = (intptr_t)status;
	}
	//we print output of threads and the final sum
	for (i=0; i<NUM_THREADS; i++){
        tot_thread = tot_thread + out_thread[i];
        printf("Thread %d: %d\n",i,out_thread[i]);
	}
    printf("Total = %d\n",tot_thread);

	return 0;
}
