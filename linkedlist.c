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

void g_hash_table_adapter_to_linked_list(char *seq,value *entry,lista_ligada *l){
	if(entry->qnt_relativa > 0 + 1e-7){
		lista_ligada* novo;
		
		novo = criar_elemento_senso(seq);
		novo->pares = entry->pares;
		novo->qsenso = entry->qsenso;
		novo->qasenso = entry->qasenso;
		novo->qnt_relativa = entry->qnt_relativa;
		
		adicionar_elemento(l,novo);
	}
	return;
}

void remover_elemento(lista_ligada *atual, lista_ligada *anterior){
	anterior->prox = atual->prox;
	free(atual->senso);
	free(atual);
	atual = anterior->prox;
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
		
	quicksort(vetor,0,tam-1);
	
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

