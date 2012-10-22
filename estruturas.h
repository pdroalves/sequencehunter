//      estruturas.h
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Estruturas comuns ao projeto Sequence Hunter
//
//		27/03/2012

#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H
#define SENSO 1
#define ANTISENSO 2
#define GATHERING_DONE -1
#define buffer_size 512 // Capacidade máxima do buffer
#define FILA_MIN 10000 // Tamanho minimo da fila antes de começar a esvazia-la
char **data;

struct vertice_grafo{
		char vertice;
		int *s_marcas;
		int *as_marcas;
		struct vertice_grafo* a;
		struct vertice_grafo* c;
		struct vertice_grafo* g;
		struct vertice_grafo* t;
};
typedef struct vertice_grafo vgrafo;


struct fila_item{
	char *seq;
	struct fila_item *prox;//Ponteiro para o item seguinte
};
typedef struct fila_item FilaItem;

struct fila{
	FilaItem *first;//Primeiro elemento da fila
	FilaItem *end;//Ultimo elemento da fila
	int size;//Elementos na fila
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

struct Value{
	int pares;
	int qsenso;
	int qasenso;
	float qnt_relativa;
};
typedef struct Value value;

struct buffer{
	char **seq;
	int capacidade;
	int load;//Guarda a quantidade de sequências carregadas;
};
typedef struct buffer Buffer;

struct despareados{
       int sensos;
       int antisensos;
};
typedef struct despareados Despareados;

struct resultfiles{
	FILE *file1;
	FILE *file299;
	FILE *file100999;
	FILE *file10009999;
	FILE *file10000;
};
typedef struct resultfiles ResultFiles;


#endif
