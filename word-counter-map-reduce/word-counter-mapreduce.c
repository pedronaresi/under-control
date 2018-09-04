//Everton da Silva Coelho  101937
//Gabriel Borin Takahashi 101953
//Pedro Manhez Naresi 105615

//Usar funcoes: stat() para ver tamanho do arquivo
//              lseek() para definir o cursor do arquivo (onde abre o arquivo)
// Abrir o arquivo nas threads, nao na master.

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUF 1024
#define NW 3

void *buffer;
int size;
pthread_barrier_t bar_read, bar_write;

typedef struct{
	char *file_name;
	long inicio;
	long fim;
}map_t;

typedef struct{
	char chave[50];
	int valor;
}map_c;

map_c maps[5*MAX_BUF];

int cont; // offset escrita map

sem_t cheio; //0: sincroniza producao, determina elementos a serem consumidos
sem_t vazio; //N: sincroniza consumo, determina elementos a serem poduzidos
sem_t mutex_map; //1
sem_t mutex_cons; //1

void inicializa(void){
	sem_init(&cheio, 0, 0);
	sem_init(&vazio, 0, NW);
	sem_init(&mutex_map, 0, 1);
	sem_init(&mutex_cons, 0, 1);
	cont = 0;
}

void map(char *buffer){
	char* token;
	char* separador = {" "};//,.:;/{}[]()!?_-\t\r\n

	token = strtok(buffer, separador);

	while(token != NULL){
		//maps[cont] = (char*) calloc(sizeof(token)*sizeof(char*), 1);

		sem_wait(&mutex_map);
		printf("BUFFER: %s\n", token);
		strcpy(maps[cont].chave, token);
		maps[cont].valor=1;
		cont++;
		sem_post(&mutex_map);
		token = strtok(NULL, separador);

	}
}

void *worker(void *args){
	map_t *dst = (map_t *)args;
	int fd_in, *ret;
	long tam = dst->fim - dst->inicio + 1;
	char buffer[tam];
	ret = (int *)calloc(1, sizeof(int));
	printf("file_name2: %s\n", dst->file_name);

	sem_wait(&mutex_cons);
	fd_in = open(dst->file_name, O_RDONLY);
	sem_post(&mutex_cons);
	if (fd_in < 0) {
		*ret = -1;
		pthread_exit(ret);
	}
	lseek(fd_in, dst->inicio,SEEK_SET);
	read(fd_in, buffer, tam);
	printf("%s\n\n\n", buffer);
	map(buffer);

	/*while(size > 0){
		size = read(fd_in, buffer, MAX_BUF);
		printf("T_%ld chegou na barreira READ\n",
			(long)pthread_self());
		pthread_barrier_wait(&bar_read);
		if (size <= 0) break;
		printf("T_%ld chegou na barreira WRITE %d\n",
			(long)pthread_self(), size);
		pthread_barrier_wait(&bar_write);
	}*/
	close(fd_in);
	*ret = 0;
	pthread_exit(ret);
}

void *master(void *args) {
	char *src = (char *)args;
	int fd_in;
	fd_in = open(src, O_RDONLY);
	int *ret = (int *)calloc(1, sizeof(int));

	struct stat buf;
	stat(src, &buf);
	printf("tam: %li\n", buf.st_size);

	long extremos=buf.st_size/3;

	map_t argumentos[NW];
	argumentos[0].inicio = 0;
	argumentos[0].fim = extremos;
	argumentos[1].inicio = extremos+1;
	argumentos[1].fim = extremos*2;
	argumentos[2].inicio = (extremos*2)+1;
	argumentos[2].fim = buf.st_size;
	argumentos[0].file_name = src;
	argumentos[1].file_name = src;
	argumentos[2].file_name = src;

	int i;
	for(i=0; i<NW; i++){
		printf("intervalo %d: %li\n", i, argumentos[i].inicio);
		printf("intervalo %d: %li\n", i, argumentos[i].fim);
	}

	pthread_t *tw;

	tw = (pthread_t *)calloc(1, NW * sizeof(pthread_t));
	for (i=0; i<NW; i++) {
		pthread_create(&tw[i], NULL, worker, &argumentos[i]);
	}
	for (i=0; i<NW; i++) {
		pthread_join(tw[i], NULL);
	}

	close(fd_in);
	*ret = 0;
	pthread_exit(ret);
}

int main(int argc, char **argv) {
	//buffer = calloc(1, MAX_BUF);
	pthread_t td;
	inicializa();
	pthread_create(&td, NULL, master, argv[1]);
	printf("teste1\n");
	pthread_join(td, NULL);
	printf("teste2\n");
	return 0;
}
