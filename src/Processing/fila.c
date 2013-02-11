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

FilaItem* criar_elemento_fila(char *seq){
	FilaItem* novo;
	int n;
	
	n = strlen(seq);
	novo = NULL;
	novo = (FilaItem*)malloc(sizeof(FilaItem));
	if(novo != NULL){
		novo->seq = (char*)malloc((n+1)*sizeof(char));
		strcpy(novo->seq,seq);
	}else{
		printStringInt("Impossível alocar memória.",sizeof(FilaItem));
		printString("Encerrando.",NULL);
		exit(1);
	}
	return novo;
}


void enfileirar(Fila *f,char *seq){
		FilaItem* novo;
		//printf("Enfileirando: %s\n",seq);
		
		novo = NULL;
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
	return;
}


void enfileirar_fila(Fila *A,Fila *B){
	//Adiciona a fila B no final da fila A
	A->end = B->first;
	A->size += B->size;
	return;	
}

char* desenfileirar(Fila *f){
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
		
		return seq;
}

gboolean fila_vazia(Fila *f){
	return (f->size == 0);
}

void despejar_fila(Fila *f,FILE *file){
	while(!fila_vazia(f)){
		despejar_seq(desenfileirar(f),file);
	}
	return;
}

int tamanho_da_fila(Fila *f){
	int size;
    size = f->size;
	return size;
}

void print_fila(Fila *f){
	int i;
	char *seq;
	FilaItem *fi;
	printf("Imprimindo fila:\n");
	
    if(f->size > 0){
		fi = f->first;
		for(i=0;i<f->size;i++){
			printf("%s\n",fi->seq);
			fi = fi->prox;
		}
	}
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
