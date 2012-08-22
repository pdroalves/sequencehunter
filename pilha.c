//	      pilha.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação da estrutura "pilha". Suporta a criação de uma
//		nova pilha, empilhar elementos e desempilhar elementos.
//
//		27/03/2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"
#include "operacoes.h"


char* desempilha(pilha *tp);
int conta_posicoes(char *seq);
pilha* criar_elemento_pilha(char*);
void empilha(pilha*,pilha*);
pilha criar_pilha();
int tamanho_da_pilha(pilha*);
void destroy(pilha *tp);
int pilha_vazia(pilha *tp);


pilha criar_pilha(){
	pilha cabeca;
	cabeca.seq = (char*)malloc(5*sizeof(char));
	strcpy(cabeca.seq,"Head");
	cabeca.prox = NULL;
	return cabeca;
}

pilha* criar_elemento_pilha(char *seq){
	char *new_seq;
	pilha *elemento;
	int seq_size;
    int i;
    
   seq_size = strlen(seq);
   new_seq = (char*)malloc((seq_size+1)*sizeof(char));
   strcpy(new_seq,seq);
   
   /*for(i=0;i<seq_size && check_base_valida(seq[i]);i++);
   if(i != seq_size){
	    seq[i] = '\0';
	    seq_size = strlen(seq);
   }*/
   elemento = (pilha*) malloc (sizeof (pilha));
   elemento->seq = new_seq;
	
	return elemento;
}

void destroy(pilha *tp){
	while(tamanho_da_pilha(tp) > 0)
		desempilha(tp);
	return;
}

// Insere um elemento y na pilha tp.
void empilha (pilha *tp,pilha *novo) { 
   novo->prox  = tp->prox;
   tp->prox = novo; 
   //printf("Elemento %s empilhado.\n",novo->seq);
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
   
   if(p == NULL){
		printf("Pilha vazia.\n");
		free(tp);
	    return NULL;
   }else{
	   //Encontra o tamanho da sequência
	   seq_size = strlen(p->seq);
	   if(seq_size + 1 > 0){
		   seq = (char*)malloc((seq_size+1)*sizeof(char));
		   strcpy(seq,p->seq);
		   tp->prox = p->prox;
		   free (p);
		   return seq; 
		}else{
			return NULL;
		}	
	}
}

void despejar(pilha* p,char *filename){
	FILE *f;
	f = fopen(filename,"w+");
	
	while(pilha_vazia(p)){
			fprintf(f,"%s\n",desempilha(p));
	}
	
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

int pilha_vazia(pilha *tp){
	if(tp->prox == NULL) return 0;//Vazia
	return 1;//Não vazia
}
