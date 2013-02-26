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
	f->nome = nome;
	f->size = 0;
	init_lock(f);
	return f;
}

Event* criar_elemento_fila_item(char *seq_central,char *seq_cincoL,int tipo){
	// Essa funcao nao aloca memoria para os dados recebidos.
	// Se voce enfileirar um elemento e depois liberar alguma de suas sequencias
	// com free(), vai dar problema.
	Event* novo;
	
	novo = NULL;
	novo = (Event*)malloc(sizeof(Event));
	if(novo != NULL){
		novo->seq_central = seq_central;
		novo->seq_cincoL = seq_cincoL;
		novo->tipo = tipo;
	}else{
		printStringInt("Impossível alocar memória.",sizeof(Event));
		printString("Encerrando.",NULL);
		exit(1);
	}
	return novo;
}



void enfileirar(Fila *f,char *seq_central,char *seq_cincoL,int tipo){
	// Essa funcao nao aloca memoria para os dados recebidos.
	// Se voce enfileirar um elemento e depois liberar alguma de suas sequencias
	// com free(), vai dar problema.
		Event* novo;
		//printf("Enfileirando: %s\n",seq);
		
		novo = NULL;
		novo = criar_elemento_fila_item(seq_central,seq_cincoL,tipo);

         omp_set_lock(&f->fila_lock);
		switch(f->size){
			case 0:
				f->first->elem = novo;
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
	return;
}

Event* desenfileirar(Fila *f){
		Event *to_return;
		//printf("Desenfileirando\n");
         omp_set_lock(&f->fila_lock);
         
		if(f->size > 0){
			to_return = f->first;
			f->first = f->first->prox;
			f->size--;
		}else{
			to_return = NULL;
		}
		
         omp_unset_lock(&f->fila_lock);
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
