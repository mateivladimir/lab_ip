#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>

char **cuvinte = NULL;
char *output_filename;
char *input_index_file;
char *input_text_file;
char *char_order_file;
int P;
int nr = 0;
struct Indexi
{
	long x;
	long y;
} indexi;
struct Litere
{
	unsigned char cod;
	int freq;
	unsigned char real;
};
struct Litere litere[26];
struct Litere aux;

long N_fis;
char *fis;
char fis_real[26];

pthread_barrier_t barrier;

int dubluri(char *p)
{
	for (int i = 0; i < nr; i++)
	{
		if (strcmp(p, cuvinte[i]) == 0)
			return 1;
	}
	return 0;
}
void aplicare_strtok()
{
	char *p;
	p = strtok(fis, " ");
	while (p != NULL)
	{
		if (dubluri(p) == 0)
		{
			char **aux = (char **)realloc(cuvinte, (nr + 1) * sizeof(char *));
			cuvinte = aux;
			cuvinte[nr] = (char *)malloc(strlen(p) + 1);
			strcpy(cuvinte[nr], p);
			nr++;
		}

		p = strtok(NULL, " ");
	}
}
// ordonez crecator numerele
void ordonare_crescatoare()
{
	char *aux2 = NULL;
	for (int i = 0; i < nr; i++)
		for (int j = 0; j < nr - i - 1; j++)
		{
			if (strcmp(cuvinte[j], cuvinte[j + 1]) > 0)
			{

				aux2 = (char *)malloc(strlen(cuvinte[j] + 1));
				strcpy(aux2, cuvinte[j]);
				cuvinte[j] = (char *)malloc(strlen(cuvinte[j + 1] + 1));
				strcpy(cuvinte[j], cuvinte[j + 1]);
				cuvinte[j + 1] = (char *)malloc(strlen(aux2) + 1);
				strcpy(cuvinte[j + 1], aux2);
			}
		}
}
void ordonare_descrescatoare()
{
	for (int i = 0; i < 26; i++)
		for (int j = i + 1; j < 26; j++)
			if (litere[i].freq < litere[j].freq)
			{
				aux = litere[i];
				litere[i] = litere[j];
				litere[j] = aux;
			}
}
void *threadFunction(void *var)
{
	int thread_id = *(int *)var;
	int start = thread_id * round(N_fis / P);
	int end = fmin(N_fis, (thread_id + 1) * round(N_fis/ P));

	for (int i = start; i < end; i++)
	{
		unsigned char p;
		p = tolower(fis[i]);
		for (int i = 0; i < 26; i++)
		{
			if (litere[i].cod == p)
			{
				litere[i].freq++;
			}
		}
	}

	pthread_barrier_wait(&barrier);
	if (thread_id ==0)
	{
		ordonare_descrescatoare();
		for (int i = 0; i < 26; i++)
			litere[i].real = fis_real[i];
	}
	for (int i = start; i < end; i++)
	{
		unsigned char p = tolower(fis[i]);
		for (int j = 0; j < 26; j++)
			if (p == litere[j].cod)
			{
				fis[i] = litere[j].real;
				break;
			}
	}

	return NULL;
}
void read_cmd_args(int argc, char **argv)
{
	if (argc < 6)
	{
		printf("How to run the program: ./freqs NUM_THREADS INPUT_INDEX_FILE INPUT_TEXT_FILE CHAR_ORDER_FILE OUTPUT_FILENAME\n");
		exit(1);
	}
	P = atoi(argv[1]);
	input_index_file = argv[2];
	input_text_file = argv[3];
	char_order_file = argv[4];
	output_filename = argv[5];
}
void init()
{

	// Deschid fisierul out-delimitatori.txt, citesc si salvez valorile in structura Indexi dupa care inchid fisierul.
	FILE *f_index = fopen(input_index_file, "r");
	if (f_index == NULL)
	{
		printf("Eroare la deschiderea fisierului out-delimitatori.txt");
		return;
	}
	if (fscanf(f_index, "%ld", &indexi.x))
		;
	if (fscanf(f_index, "%ld", &indexi.y))
		;

	//alocam un char* unde vom salva ce avem in fisier
	N_fis = indexi.y - indexi.x +1;
	fis = (char *)malloc(N_fis);

	//Deschidem fisierul input.txt, citesc si salvez in fis* portiunea de interes
	FILE *f_input = fopen(input_text_file, "r");
	if (f_input == NULL)
	{
		printf("Eroare la deschiderea fisierului input.txt");
		return;
	}
	fseek(f_input, indexi.x, SEEK_SET);
	if (fread(fis, 1, N_fis -1, f_input))
		;

	//Initializam vectorul de structuri
	for (int i = 0; i < 26; i++)
	{
		litere[i].cod = (unsigned char)(i + 97);
		litere[i].freq = 0;
	}

	pthread_barrier_init(&barrier, NULL, P);

	FILE *f_real = fopen(char_order_file, "r");
	if (f_real == NULL)
		return;
	if (fread(fis_real, 1, 26, f_real))
		;
}

int main(int argc, char **argv)
{
	int i;

	read_cmd_args(argc, argv);
	init();
	pthread_t tid[P];
	int thread_id[P];
	for (i = 0; i < P; i++)
		thread_id[i] = i;
	for (i = 0; i < P; i++)
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	for (i = 0; i < P; i++)
		pthread_join(tid[i], NULL);

	aplicare_strtok();
	ordonare_crescatoare();

	FILE *f_out = fopen(output_filename, "w");
	if (f_out == NULL)
	{
		printf("Nu s-a putut face niciun fisier de output.");
		return 0;
	}
	fprintf(f_out, "%d\n", nr);
	for (int i = 0; i < nr; i++)
	{
		fprintf(f_out, "%s\n", cuvinte[i]);
	}

	return 0;
}
