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
		struct vertice_grafo* a;
		struct vertice_grafo* c;
		struct vertice_grafo* g;
		struct vertice_grafo* t;
		int psenso;
		int pasenso;
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
	int qsenso;
	int qasenso;
	struct lista_ligada* prox;
};
typedef struct lista_ligada lista_ligada;

struct buffer{
	char **seq;
	int capacidade;
};
typedef struct buffer Buffer;

#endif
