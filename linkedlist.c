//      linkedlist.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação de estruturas do tipo "lista ligada".
//		Suporta a criação de uma nova lista, adicionar novo elemento
//		ao final e todos os algoritmos de busca necessários.
//
//		27/03/2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "estruturas.h"
#include "log.h"
#include "operacoes.h"
#include "processing_data.h"
#define MIN(a,b) a>=b?b:a

lista_ligada* criar_lista(){
	lista_ligada *l;
	l = (lista_ligada*)malloc(sizeof(lista_ligada));
	l->qsenso = 0;
	l->qasenso = 0;
	l->senso = (char*)malloc(5*sizeof(char));
	strcpy(l->senso,"Head");
	l->prox = NULL;
	return l;
}

lista_ligada* criar_elemento_senso(char *seq){
	lista_ligada *novo;
	int seq_size;
	
	seq_size = strlen(seq);
	novo = (lista_ligada*)malloc(sizeof(lista_ligada));
	novo->qsenso = 1;
	novo->qasenso = 0;
	novo->senso = (char*)malloc((seq_size+1)*sizeof(char));
	strcpy(novo->senso,seq);
	novo->prox = NULL;
	return novo;
}

lista_ligada* criar_elemento_antisenso(char *seq){
	lista_ligada *novo;
	int seq_size;
	
	seq_size = strlen(seq);
	novo = (lista_ligada*)malloc(sizeof(lista_ligada));
	novo->qsenso = 0;
	novo->qasenso = 1;
	novo->senso = (char*)malloc((seq_size+1)*sizeof(char));
	strcpy(novo->senso,seq);
	novo->prox = NULL;
	return novo;
}

lista_ligada* adicionar_elemento(lista_ligada *lista,lista_ligada *novo){
	//Recebe ultimo elemento da lista ligada
	lista_ligada *tmp;
	
	tmp = lista;
	while(tmp->prox != NULL){
		tmp = tmp->prox;//Procura o ultimo elemento da fila
	}
	tmp->prox = novo;

	return tmp;
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
					//free(seq);
					return 0;
			}
			
			while(p != NULL){
				cmp = strncmp(seq,p->senso,str_size);
				if(cmp == 0){
						//Encontrou
						p->qsenso++;
						//free(seq);
						return 0;
				}
				p = p->prox;
			}
		}
		//Não encontrou e chegou ao fim da lista ligada
		
		adicionar_elemento(l,criar_elemento_senso(seq));
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
		
		adicionar_elemento(l,criar_elemento_antisenso(seq));
		return 1;
	}
	
	return 1;	
}

void qnt_relativa(lista_ligada* l){
	float total = 0;
	
	lista_ligada *p;
	
	//Contabiliza a quantidade total de pares encontrados
	p = l->prox;
	while(p !=NULL){
		p->pares = MIN(p->qsenso,p->qasenso);
		total += p->pares;
		p = p->prox;
	}
	
	//Grava quantidade relativa de cada tipo de pares encontrados
	p = l->prox;
	while(p != NULL){
		if(total > 0){
			p->qnt_relativa = p->pares / total;
		}else{
			p->qnt_relativa = 0;
		}		
		p = p->prox;
	}
	
	return;
}

lista_ligada** ordena_pares(lista_ligada* l){
	//Recebe lista com resultados encontrados, ordena e retorna vetor com os elementos de maior e menor frequencia
	lista_ligada* p;
	lista_ligada** vetor;
	int tam = 0;
	int i;
	int th_id;
	int nthreads;
	
	//Calcula tamanho da lista ligada e adiciona elemento no vetor
	p = l->prox;
	while(p != NULL){
		tam++;
		p = p->prox;
	}
	
	vetor = (lista_ligada**)malloc((tam+1)*sizeof(lista_ligada*));
	
	//Adiciona ponteiro para cada elemento em um vetor
	p = l->prox;
	for(i=0;i<tam;i++){
		vetor[i] = p;
		p = p->prox;
	}
	
	//Ordena
	#pragma omp parallel shared(vetor) shared(tam)
	{
		int razao;
		th_id = omp_get_thread_num();
		nthreads = omp_get_num_threads();
		razao = tam/nthreads;
		
		quicksort(vetor,th_id*razao,(th_id+1)*razao-1);
	}
	vetor[tam] = (lista_ligada*)malloc(sizeof(lista_ligada));
	vetor[tam]->pares = -1;
	return vetor;
}
	
Despareados* recupera_despareados(lista_ligada *l){
	lista_ligada *atual,*anterior;
	int sensos_solitarios = 0;
	Despareados *desp;
	int diff;
	
	desp = (Despareados*)malloc(sizeof(Despareados));
	
	anterior = l;
	if(anterior != NULL){
		atual = anterior->prox;
		while(atual != NULL){
			diff = atual->qsenso - atual->qasenso;
			printf("%s - S:%d - As:%d\n",atual->senso,atual->qsenso,atual->qasenso);
			print_despareadas(atual->senso,atual->qsenso,atual->qasenso);
			if(diff != 0){
				 if(diff > 0) desp->sensos+=diff;
				 else desp->antisensos+=-diff;
			}
			anterior = atual;
			atual = atual->prox;
		}
	}
	
	return desp;
}

void imprimir_sensos(lista_ligada **resultados){
	int i;
	char string_fim[4] = "FIM";
	i=0;
	
	if(resultados[0]->senso != NULL){
		print_resultados(resultados);
		while(resultados[i]->pares != -1){
			if(resultados[i]->pares != 0)
			printf("	%s x%d => %.3f \%\n",resultados[i]->senso,resultados[i]->pares,resultados[i]->qnt_relativa*100);
			i++;
		}
	}
	return;
}
