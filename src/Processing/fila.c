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
#include "../Headers/estruturas.h"
#include "../Headers/operacoes.h"
#include "../Headers/load_data.h"
#include "../Headers/log.h"

void init_lock(Fila* f){
	omp_init_lock(&f->fila_lock);
}

Fila* criar_fila(char *nome){
	Fila *f;
	f = (Fila*)malloc(sizeof(Fila));
	if(f){
		f->nome = nome;
		f->size = 0;
		init_lock(f);
		f->first = NULL;
		f->end = NULL;
	}
	return f;
}

Event* criar_elemento_fila_event(char *seq_central,char *seq_cincoL,int tipo){
	// Essa funcao nao aloca memoria para os dados recebidos.
	// Se voce enfileirar um elemento e depois liberar alguma de suas sequencias
	// com free(), vai dar problema.
	Event* novo;
	
	novo = (Event*)malloc(sizeof(Event));
	if(novo != NULL){
		novo->seq_central = seq_central;
		novo->seq_cincoL = seq_cincoL;
		novo->tipo = tipo;
	}else{
		printStringInt("Memory Error.",sizeof(Event));
		printString("Aborting.",NULL);
		exit(1);
	}
	return novo;
}


void enfileirar(Fila *f,void *data){
	// Essa funcao nao aloca memoria para os dados recebidos.
	// Se voce enfileirar um elemento e depois liberar alguma de suas sequencias
	// com free(), vai dar problema.
		FilaItem* novo;
		int size;
		//printf("Enfileirando: %s\n",seq);
		
		novo = (FilaItem*)malloc(sizeof(FilaItem));
		if(novo != NULL){
			novo->data = data;

			size = tamanho_da_fila(f);
			 omp_set_lock(&f->fila_lock);
			switch(size){
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
			omp_unset_lock(&f->fila_lock);
			
		}
	return;
}

Fila* desenfileirar(Fila *f){
	Fila *to_return;
	FilaItem *hold;
	
	//printf("Desenfileirando\n");
	 
	if(tamanho_da_fila(f) > 0){
		omp_set_lock(&f->fila_lock);
		hold = f->first;		
		f->first = f->first->prox;
		to_return = hold->data;	
		if(hold != NULL)
			free(hold);
		
		f->size--;
		omp_unset_lock(&f->fila_lock);
	}else{
		to_return = NULL;
	}
	
	return to_return;
}


gboolean fila_vazia(Fila *f){
	return (tamanho_da_fila(f) == 0);
}

int tamanho_da_fila(Fila *f){
	int size;
   omp_set_lock(&f->fila_lock);
    size = f->size;
   omp_unset_lock(&f->fila_lock);
	return size;
}

void destroy(Fila *f){
	if(f->size != 0){
		FilaItem *hold;
		while(!fila_vazia(f)){
			hold = desenfileirar(f);
			free(hold);
		}
	}
	
	omp_destroy_lock(&f->fila_lock);
	free(f);
	return;
}
