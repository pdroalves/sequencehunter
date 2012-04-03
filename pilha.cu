//	      pilha.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação da estrutura "pilha". Suporta a criação de uma
//		nova pilha, empilhar elementos e desempilhar elementos.
//
//		27/03/2012

#include<stdio.h>
#include<string.h>
#include "estruturas.h"
#include "operacoes.h"


char* desempilha(pilha *tp);
int conta_posicoes(char *seq);
void empilha(char *seq,pilha *tp);
pilha criar_pilha();
int tamanho_da_pilha(pilha*);
void destroy(pilha *tp);


pilha criar_pilha(){
	pilha cabeca;
	cabeca.prox = NULL;
	return cabeca;
}

void destroy(pilha *tp){
	while(tamanho_da_pilha(tp) > 0)
		desempilha(tp);
	return;
}

// Insere um elemento y na pilha tp.
void empilha (char *seq, pilha *tp) { 
   pilha *nova;
   int seq_size;//A sequência seq possui pelo menos o elemento \0
   
   //Encontra o tamanho da sequência
   seq_size = strlen(seq);
   
   nova = (pilha*) malloc (sizeof (pilha));
   nova->seq = (char*) malloc(seq_size*sizeof(char));
   
   memcpy(nova->seq,seq,seq_size);
   nova->prox  = tp->prox;
   tp->prox = nova; 
   return;
}

// Remove um elemento da pilha tp.
// Supõe que a pilha não está vazia. 
// Devolve o elemento removido.
char* desempilha (pilha *tp) {
   pilha *p;
   char *seq;
   int seq_size;
   
   p = tp->prox;
   
   if(p == NULL) return NULL;
   
   //Encontra o tamanho da sequência
   seq_size = strlen(p->seq);
   
   seq = (char*)malloc(seq_size*sizeof(char));
   memcpy(seq,p->seq,seq_size*sizeof(char));
   
   tp->prox = p->prox;
   free (p);
   return seq; 
}

int tamanho_da_pilha(pilha *tp){
	int p;
	pilha *tmp;

	p = 0;
	tmp = tp;

	while(tmp->prox != NULL){
		tmp = tmp->prox;
		p++;
	}

	return p;
}
