//      estruturas.h
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Estruturas comuns ao projeto Sequence Hunter
//
//		27/03/2012

#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

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

struct pilha{
	//Implementação usando listas ligadas
	char *seq;
	struct pilha *prox;
};
typedef struct pilha pilha;

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

#endif
