//      linkedlist.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação de estruturas do tipo "lista ligada".
//		Suporta a criação de uma nova lista, adicionar novo elemento
//		ao final e todos os algoritmos de busca necessários.
//
//		27/03/2012

#include<stdio.h>
#include "estruturas.h"
#include "operacoes.h"

lista_ligada criar_lista(){
	lista_ligada l;
	return l;
}

void adicionar_elemento(lista_ligada *final,char *seq){
	//Recebe ultimo elemento da lista ligada
	int i;
	int seq_size;
	lista_ligada *novo;
	
		
	//Cria novo elemento para a lista ligada
	novo = (lista_ligada*)malloc(sizeof(lista_ligada));	
	seq_size = strlen(seq);
	novo->senso = (char*)malloc(seq_size*sizeof(char));
	memcpy(novo->senso,seq,seq_size);
	novo->qsenso = 1;
	novo->qasenso = 0;
	
	final->prox = novo;//Adiciona novo elemento no final
	return;
}

int busca_lista_s(lista_ligada *l, char *seq){
	//Busca por determinada sequência senso nas listas ligadas
	//Retorna 0 se encontrar
	//Retorna 1 se não enccontrar e tiver de adiciona-lo no final da lista
	lista_ligada *p;
	int cmp;
	
	p = l;	
	cmp = strcmp(seq,p->senso);
	if(cmp == 0){
			//Encontrou
			p->qsenso++;
			return 0;
	}
	
	while(p->prox != NULL){
		cmp = strcmp(seq,p->senso);
		if(cmp == 0){
				//Encontrou
				p->qsenso++;
				return 0;
		}
	}
	
	//Não encontrou e chegou ao fim da lista ligada
	adicionar_elemento(p,seq);
	
	return 1;	
}

int busca_lista_as(lista_ligada *l, char *seq){
	//Busca por determinada sequência antisenso nas listas ligadas
	//Retorna 0 se encontrar
	//Retorna 1 se não enccontrar
	lista_ligada *p;
	int cmp;
	
	p = l;	
	cmp = strcmp(seq,p->senso);
	if(cmp == 0){
			//Encontrou
			p->qasenso++;
			return 0;
	}
	
	while(p->prox != NULL){
		cmp = strcmp(seq,p->senso);
		if(cmp == 0){
				//Encontrou
				p->qasenso++;
				return 0;
		}
	}
	
	//Não encontrou e chegou ao fim da lista ligada
	//Não faz nada
	
	return 1;	
}
