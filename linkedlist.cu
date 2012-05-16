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

#define MIN(a,b) a>=b?b:a

lista_ligada* criar_lista(){
	lista_ligada *l;
	l = (lista_ligada*)malloc(sizeof(lista_ligada));
	l->qsenso = 0;
	l->qasenso = 0;
	l->senso = (char*)calloc('\0',5*sizeof(char));
	strcat(l->senso,"Head");
	return l;
}

lista_ligada* criar_elemento_lista(char *seq){
	lista_ligada *novo;
	int seq_size;
	
	seq_size = strlen(seq);
	novo = (lista_ligada*)malloc(sizeof(lista_ligada));
	novo->qsenso = 1;
	novo->qasenso = 0;
	novo->senso = (char*)calloc('\0',(seq_size+1)*sizeof(char));
	memcpy(novo->senso,seq,seq_size+1);
	novo->prox = NULL;
	return novo;
}

void adicionar_elemento(lista_ligada *lista,lista_ligada *novo){
	//Recebe ultimo elemento da lista ligada
	lista_ligada *tmp;
	
	tmp = lista;
	while(tmp->prox != NULL){
		tmp = tmp->prox;//Procura o ultimo elemento da fila
	}
	tmp->prox = novo;

	return;
}

void remover_elemento(lista_ligada *atual, lista_ligada *anterior){
	anterior->prox = atual->prox;
	free(atual->senso);
	free(atual);
	atual = anterior->prox;
	return;
}

int busca_lista_s(lista_ligada *l, char *seq){
	//Busca por determinada sequência senso nas listas ligadas
	//Retorna 0 se encontrar ou se seq for nulo
	//Retorna 1 se não encontrar e tiver de adiciona-lo no final da lista
	lista_ligada *p;
	int cmp;
	int str_size;
	
	str_size = strlen(seq);
	
	if(seq != NULL){
		if(l->prox != NULL){//Lista vazia
			p = l->prox;	
			cmp = strncmp(seq,p->senso,str_size);
			if(cmp == 0){
					//Encontrou
					p->qsenso++;
					return 0;
			}
			
			while(p != NULL){
				cmp = strncmp(seq,p->senso,str_size);
				if(cmp == 0){
						//Encontrou
						p->qsenso++;
						return 0;
				}
				p = p->prox;
			}
		}
		//Não encontrou e chegou ao fim da lista ligada
		
		adicionar_elemento(l,criar_elemento_lista(seq));
		return 1;
	}
	
	return 0;	
}

int busca_lista_as(lista_ligada *l, char *seq){
	//Busca por determinada sequência antisenso nas listas ligadas
	//Retorna 0 se encontrar
	//Retorna 1 se não enccontrar
	lista_ligada *p;
	int cmp;
	
	if(seq != NULL){
		if(l->prox != NULL){//Lista vazia
			p = l->prox;	
			cmp = strcmp(seq,p->senso);
			if(cmp == 0){
					//Encontrou
					p->qasenso++;
					return 0;
			}
			
			while(p != NULL){
				cmp = strcmp(seq,p->senso);
				if(cmp == 0){
						//Encontrou
						p->qasenso++;
						return 0;
				}
				p = p->prox;
			}
		}
		
		return 1;
	}
	
	return 1;	
}

int limpando_sensos(lista_ligada *l){
	lista_ligada *atual,*anterior;
	int sensos_solitarios = 0;
	
	anterior = l;
	if(anterior != NULL){
		atual = anterior->prox;
		while(atual != NULL){
			if(atual->qsenso != atual->qasenso){
				 printf("%s x%d\n",atual->senso,abs(atual->qsenso - atual->qasenso));
				 //remover_elemento(atual,anterior);
				 sensos_solitarios++; 
			}
			anterior = atual;
			atual = atual->prox;
		}
	}
	
	return sensos_solitarios;
}

void imprimir_sensos(lista_ligada *l){
	
	lista_ligada *p;
	
	p = l->prox;
	
	while(p !=NULL){
		printf("	%s x%d\n",p->senso,MIN(p->qsenso,p->qasenso));
		p = p->prox;
	}
}
