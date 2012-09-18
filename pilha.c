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
#include <omp.h>
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
	cabeca.size=0;
	cabeca.prox = NULL;
	return cabeca;
}

pilha* criar_elemento_pilha(char *seq){
	char *new_seq;
	pilha elemento;
	int seq_size;
    int i;
    
   seq_size = strlen(seq);
   new_seq = (char*)malloc((seq_size+1)*sizeof(char));
   strcpy(new_seq,seq);
   
   for(i=0;i<seq_size && check_base_valida(seq[i]);i++);
   if(i != seq_size){
	    seq[i] = '\0';
	    seq_size = strlen(seq);
   }
   
	elemento.seq = new_seq;
	
	return &elemento;
}

void destroy(pilha *tp){
	char *seq;
	while(tamanho_da_pilha(tp) > 0){
		seq = desempilha(tp);
		if(seq != NULL) free(seq);
	}
	return;
}

// Insere um elemento novo na pilha tp.
void empilha (pilha *tp,pilha *novo) { 
   novo->prox  = tp->prox;
   tp->prox = novo; 
   tp->size++;
   //printf("Elemento %s empilhado.\n",novo->seq);
   return;
}

// Conecta as duas pilhas mantendo a pilha A no começo

void empilhar_pilha(pilha *A,pilha *B){
	// Procura o ultimo elemento da pilha A
	pilha *p;
	p = A;
	while(p->prox != NULL){
		p = p->prox;
	}
	
	//Linka as duas listas
	p->prox = B->prox;
	A->size+=B->size;
	//free(B);
	
}

// Remove um elemento da pilha tp.
// Supõe que a pilha não está vazia. 
// Devolve o elemento removido.
char* desempilha (pilha *tp) {
   pilha *p;
   char *seq;
   int seq_size;
   #pragma omp atomic
   {
   p = tp->prox;
   
   if(p == NULL){
		// Pilha vazia
	    return NULL;
   }else{
	   //Encontra o tamanho da sequência
		seq = p->seq;
		tp->prox = p->prox;  
		tp->size--;
		return seq; 
	}	
}
}

void despejar(pilha* p,FILE *f){
	char *seq;	
	while(pilha_vazia(p)){
			seq = desempilha(p);
			fputs(seq,f);
			free(seq);
	}

	rewind(f);
	return ;
}

void despejar_seq(char *seq,FILE *f){
	fputs(seq,f);
	//if(seq != NULL)
		//free(seq);
	return;
}

char* carrega_do_arquivo(int n,FILE *filename){
	char *seq;
	seq = (char*)malloc((n+1)*sizeof(char));
	if(!feof(filename)){
		fgets(seq,n+1,filename);
		return seq;
	}else return NULL;
}

void carregar_pilha(pilha *p,char *filename){
	FILE *f;
	f = fopen(filename,"r");


}

int tamanho_da_pilha(pilha *tp){
	return tp->size;
}

int pilha_vazia(pilha *tp){
	if(tp->prox == NULL) return 0;//Vazia
	return 1;//Não vazia
}
