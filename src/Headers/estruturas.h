//      estruturas.h
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Estruturas comuns ao projeto Sequence Hunter
//
//		27/03/2012

#ifndef ESTRUTURAS_H
#include <omp.h>
#define ESTRUTURAS_H
#define SENSO 1
#define ANTISENSO 2
#define GATHERING_DONE -1
#define FILA_MIN 5000 // Tamanho minimo da fila antes de começar a esvazia-la
#define MAX_CUDA_THREADS_PER_BLOCK 608
#define MAX_SEQ_SIZE 1000
#define MAX_FILA_SIZE 100000
#define MAX_SOCKET_MSG_SIZE 1000

// Socket msgs
#define SKT_MSG_HELLO "hello"
#define SKT_MSG_CLOSE "bye"
#define SKT_MSG_GETSIZE "size"
#define SKT_MSG_PROCESS "processDB"
#define SKT_MSG_ABORT "abort"

struct event{
	char *seq_central;
	char *seq_cincoL;
	int tipo;
};
typedef struct event Event;

struct fila_item{
	void* data;
	struct fila_item *prox;//Ponteiro para o item seguinte
};
typedef struct fila_item FilaItem;

struct fila{
	FilaItem *first;//Primeiro elemento da fila
	FilaItem *end;//Ultimo elemento da fila
	char *nome;
	int size;//Elementos na fila
	omp_lock_t fila_lock;
};
typedef struct fila Fila;

struct lista_ligada{
	char *senso;
	int pares;
	int qsenso;
	int qasenso;
	float qnt_relativa;
	struct lista_ligada* prox;
};
typedef struct lista_ligada lista_ligada;

struct buffer{
	char **seq;
	int capacidade;
	int load;//Guarda a quantidade de sequências carregadas;
};
typedef struct buffer Buffer;

struct value{
	int qsensos;
	int qasensos;
};
typedef struct value Valor;

struct pares{
    int sensos;
    int antisensos;
};
typedef struct pares Pares;

struct params{
	int verbose;
	int silent;
	int debug;
	int cut_central;
	int gui_run;
	int dist_regiao_5l;
	int tam_regiao_5l;
};
typedef struct params Params;

struct socket{
	struct _GSocketConnection * connection;
	struct _GSocketClient * client;
	struct _GInputStream * istream;
	struct _GOutputStream * ostream;	
};
typedef struct socket Socket;

#endif
