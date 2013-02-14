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
#include <time.h>
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include "../Headers/operacoes.h"
#include "../Headers/processing_data.h"
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
	novo->prox = lista->prox;
	lista->prox = novo;
	return lista;
}

void remover_elemento(lista_ligada *atual, lista_ligada *anterior){
	anterior->prox = atual->prox;
	free(atual->senso);
	free(atual);
	atual = anterior->prox;
	return;
}

lista_ligada* get_biggest(lista_ligada *l){
	// Suponhe a lista l nao vazia, retorna o elemento com a maior quantidade de pares e o remove da lista original
	lista_ligada *tmp,*dir_tmp,*esq_tmp;
	lista_ligada *maior,*dir_maior,*esq_maior;
	
	if(l->prox == NULL) return NULL;
	
	// Toma o primeiro elemento como o com mais pares e marca os elementos a direita e a esquerda
	maior = l->prox;
	dir_maior = NULL;
	esq_maior = maior->prox;	
	
	// Se houver apenas um elemento na lista, retorna ele mesmo
	if(maior->prox == NULL){
		l->prox = NULL;
		return maior;
	}
	
	// Itera ateh o final da lista
	tmp = maior->prox;
	dir_tmp = maior;
	esq_tmp = tmp->prox;
	
	while(tmp->prox != NULL){
		if(tmp->pares > maior->pares){
			// Achei um maior do que o maior
			maior = tmp;
			dir_maior = dir_tmp;
			esq_maior = esq_tmp;
		}
		dir_tmp = tmp;
		tmp = tmp->prox;
		if(tmp->prox != NULL)
			esq_tmp = tmp->prox;
		else
			esq_tmp = NULL;
	}
	
	if(tmp->pares > maior->pares){
		maior = tmp;
		dir_maior = dir_tmp;
		if(esq_tmp != NULL)
			esq_maior = esq_tmp;
		else
			esq_maior = NULL;
	}
	if(dir_maior == NULL){
		if(esq_maior != NULL)
			l->prox = esq_maior;
		else
			l->prox = NULL;
	}else{
		if(esq_maior != NULL)
			dir_maior->prox = esq_maior;
		else
			dir_maior->prox = NULL;
	}
	return maior;
}

lista_ligada* ordena_pares(lista_ligada* l,int max_events){
	// Recebe lista com resultados encontrados, ordena e retorna uma lista ligada ordenada
	// Lida apenas com os max_events maiores resultados
	
	lista_ligada *resultados;
	int i=0;
	
	resultados = criar_lista();
	
	while(i < max_events && get_size(l) > 0){
		adicionar_elemento(resultados,get_biggest(l));
		i++;
	}
	
	return resultados;
}
	
Pares* recupera_despareados(lista_ligada *l){
	lista_ligada *atual,*anterior;
	int sensos_solitarios = 0;
	Pares *desp;
	int diff;
	
	desp = (Pares*)malloc(sizeof(Pares));
	
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

void imprimir_lista_ligada(lista_ligada *resultados,char *tempo,gboolean silent,gboolean gui_run){
	lista_ligada *p;
	int i;	
	FILE *out;
	char outname_tmp[100];
	char outname[100];
	
	tempo[strlen(tempo)-1] = '\0';
	strcpy(outname_tmp,"SHunter Resultados - ");
	strcat(outname_tmp,tempo);
	strcat(outname_tmp,".txt");

	for(i=0;i <= strlen(outname_tmp); i++){
		if(outname_tmp[i] == ':')
			outname[i] = ' ';
		else
			outname[i] = outname_tmp[i];
	}

	out = fopen(outname,"a");
	if(!out){
		printString("Resultados não puderam ser salvos em ",outname);
		return;
	}

	fprintf(out,"################### %s ###################\n\n",tempo); 
	
	p = resultados->prox;
	while(p != NULL){
		if(!silent)
			printf("	%s x%d => %.3f \%, S:%d - AS: %d\n",p->senso,p->pares,p->qnt_relativa*100,p->qsenso,p->qasenso);
		fprintf(out,"%s		%d\n",p->senso,p->pares);
		print_resultado(p);
		p = p->prox;
	}
	return;
}

int get_size(lista_ligada *l){
	lista_ligada *p;
	int tam = 0;
		
	//Calcula tamanho da lista ligada e adiciona elemento no vetor
	p = l->prox;
	while(p != NULL){
		tam++;
		p = p->prox;
	}
	
	return tam;
}
