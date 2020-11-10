#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

char *output_filename;
int P;

void *threadFunction(void *var) {
	int thread_id = *(int*) var;
	return NULL;
}

void read_cmd_args(int argc, char **argv) {
	if (argc < 5) {
		printf("How to run the program: ./search NUM_THREADS INPUT_VALS_FILE INPUT_TEXT_FILE OUTPUT_FILENAME\n");
		exit(1);
	}
	P = atoi(argv[1]);
	(argv[2]);
	(argv[3]);
	output_filename = argv[4];
}

int main(int argc, char **argv) {
	int i;

	read_cmd_args(argc, argv);

	pthread_t tid[P];
	int thread_id[P];
	for (i = 0; i < P; i ++)
		thread_id[i] = i;
	for (i = 0; i < P; i ++)
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	for (i = 0; i < P; i ++)
		pthread_join(tid[i], NULL);

	return 0;
}
