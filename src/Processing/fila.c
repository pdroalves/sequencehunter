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

Fila* criar_fila(char *nome){
	Fila *f;
	f = (Fila*)malloc(sizeof(Fila));
	f->nome = nome;
	f->size = 0;
	return f;
}

FilaItem* criar_elemento_fila(char *seq_central,char *seq_cincoL,int tipo){
	// Essa funcao nao aloca memoria para os dados recebidos.
	// Se voce enfileirar um elemento e depois liberar alguma de suas sequencias
	// com free(), vai dar problema.
	FilaItem* novo;
	
	novo = NULL;
	novo = (FilaItem*)malloc(sizeof(FilaItem));
	if(novo != NULL){
		novo->seq_central = seq_central;
		novo->seq_cincoL = seq_cincoL;
		novo->tipo = tipo;
	}else{
		printStringInt("Impossível alocar memória.",sizeof(FilaItem));
		printString("Encerrando.",NULL);
		exit(1);
	}
	return novo;
}


void enfileirar(Fila *f,char *seq_central,char *seq_cincoL,int tipo){
	// Essa funcao nao aloca memoria para os dados recebidos.
	// Se voce enfileirar um elemento e depois liberar alguma de suas sequencias
	// com free(), vai dar problema.
	
		FilaItem* novo;
		//printf("Enfileirando: %s\n",seq);
		
		novo = NULL;
		novo = criar_elemento_fila(seq_central,seq_cincoL,tipo);

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
	return;
}

FilaItem* desenfileirar(Fila *f){
		FilaItem *to_return;
		//printf("Desenfileirando\n");
		
		if(f->size > 0){
			to_return = f->first;
			f->first = f->first->prox;
			f->size--;
		}
		
	return to_return;
}

gboolean fila_vazia(Fila *f){
	return (f->size == 0);
}

int tamanho_da_fila(Fila *f){
	int size;
    size = f->size;
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
