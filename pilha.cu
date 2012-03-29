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

pilha criar_pilha(){
	pilha cabeca;
	cabeca->prox = NULL;
	return cabeca;
}

// Insere um elemento y na pilha tp.
void empilha (char *seq, pilha *tp) { 
   pilha *nova;
   int i = 0;
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
void desempilha (char *seq,pilha *tp) {
   pilha *p;
   int seq_size;
   int i;
   
   p = tp->prox;
   
   //Encontra o tamanho da sequência
   seq_size = strlen(seq);
   
   seq = (char*)malloc(seq_size*sizeof(char));
   memcpy(seq,p->seq[i],seq_size);
   
   tp->prox = p->prox;
   free (p);
   return; 
}
