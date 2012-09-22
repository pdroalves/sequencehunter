//	      fila.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementacao da estrutura "Fila". Suporta a adicao de elementos
//		no final da fila, a remocao dos elementos no comeco e outras
//		operacoes necessarias para o processamento das sequencias.
//
//		27/03/2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <glib.h>
#include "estruturas.h"
#include "operacoes.h"
#include "load_data.h"

omp_lock_t fila_lock;

Fila* criar_fila(){
	Fila *f;
	f = (Fila*)malloc(1+sizeof(Fila));
	f->size = 0;
	return f;
}

void start_fila_lock(){
	omp_init_lock (&fila_lock);
	return;
}

FilaItem* criar_elemento_fila(char *seq){
	FilaItem* novo;
	int n;
	
	n = strlen(seq);
	novo = (FilaItem*)malloc(sizeof(FilaItem));
	novo->seq = (char*)malloc((n+1)*sizeof(char));
	
	strcpy(novo->seq,seq);
	
	return novo;
}


void enfileirar(Fila *f,char *seq){
         omp_set_lock(&fila_lock);
		FilaItem* novo;
		//printf("Enfileirando\n");
		novo = criar_elemento_fila(seq);
		
		switch(f->size){
			case 0:
				f->first = novo;
			break;
			case 1:
				f->end = novo;
				f->first->prox = f->end;
			break;
			default:
				f->end->prox = novo;
				f->end = f->end->prox;
			break;
		}
		
		f->size = f->size + 1;
	
         omp_unset_lock(&fila_lock);
	return;
}

void enfileirar_fila(Fila *A,Fila *B){
	//Adiciona a fila B no final da fila A
	A->end = B->first;
	A->size += B->size;
	return;	
}

char* desenfileirar(Fila *f){
         omp_set_lock(&fila_lock);
		FilaItem *hold;
		char *seq;
		//printf("Desenfileirando\n");
		
		if(f->size > 0){
			hold = f->first;
			seq = f->first->seq;
			f->first = f->first->prox;
			free(hold);
			f->size--;
		}
		
         omp_unset_lock(&fila_lock);
		return seq;
}

gboolean fila_vazia(Fila *f){
	return (f->size == 0);
}

void despejar_fila(Fila *f,FILE *file){
	while(!fila_vazia){
		despejar_seq(desenfileirar(f),file);
	}
	return;
}

int tamanho_da_fila(Fila *f){
	int size;
    omp_set_lock(&fila_lock);
    size = f->size;
    omp_unset_lock(&fila_lock);
	return size;
}


void destroy(Fila *f){
	if(f->size != 0){
		char *hold;
		while(!fila_vazia(f)){
			hold = desenfileirar(f);
			free(hold);
		}
	}
	
	free(f);
	return;
}
