/* This file was automatically generated.  Do not edit! */
#if !defined(ESTRUTURAS_H)
#define ESTRUTURAS_H
#endif
#if !defined(ESTRUTURAS_H)
typedef struct pilha pilha;
struct pilha {
	//Implementação usando listas ligadas
	char *seq;
	struct pilha *prox;
};
#endif
void desempilha(char *seq,pilha *tp);
int conta_posicoes(char *seq);
void empilha(char *seq,pilha *tp);
pilha criar_pilha();
