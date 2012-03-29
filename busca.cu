//      busca.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação do algoritmo de busca por grafos.
//
//		27/03/2012

#include <stdio.h>
#include <cuda.h>
#include "estruturas.h"


__global__ void set_grafo(char*,char *,vgrafo*,vgrafo*,vgrafo*, vgrafo*);

__host__ __device__ void caminhar(vgrafo*, vgrafo*, int*,int*);
__device__ void build_grafo(vgrafo*,vgrafo*,vgrafo*, vgrafo*);
__host__ __device__ vgrafo* busca_vertice(char,vgrafo *,vgrafo *,vgrafo *, vgrafo *);

__global__ void k_busca(int *matchs,char **data,int size,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
	
	int posicao = blockIdx.x*blockDim.x + threadIdx.x;
	char *seq;
	int i=0;
	int found = 0;//1 se encontrar uma seq
	int s_match = 0;
	int as_match = 0;
	vgrafo *atual;
	vgrafo *prox;
	
	seq =  data[posicao];
	#pragma unroll 1
	while(seq[i+1] != '\0' && s_match < (size-1) && as_match < (size-1)){
		atual = busca_vertice(seq[i],a,c,g,t);
		prox = busca_vertice(seq[i+1],a,c,g,t);
		caminhar(atual,prox,&s_match,&as_match);
		i++;
	}
	
	//Marca o que foi encontrado
	//1 se for senso
	//2 se for antisenso
	//0 se não for nada
	matchs[posicao] = s_match / (size-1);
	matchs[posicao] = 2*(as_match / (size-1));
	
	return;
}

__host__ __device__ vgrafo* busca_vertice(char base,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
	//Funcao temporária. Ficará aqui até eu pensar em algo melhor
	switch(base){
		case 'A':
			return a;
		case 'C':
			return c;
		case 'G':
			return g;
		case 'T':
			return t;
	}
	
	return NULL;
}

__host__ __device__ void caminhar(vgrafo *atual, vgrafo *prox, int *s_match,int *as_match){ 
	//Recebe o vertice atual
	//Recebe o próximo vertice
	//Recebe um contador de bases acertadas para a sequencia senso s_match
	//Recebe um contador de bases acertadas para a sequencia antisenso as_match
	
	//OTIMIZAR! Tem muitos IFs
	
	
	if(prox->psenso == atual->psenso+1)//A sequencia confere com o senso
		s_match++;
	else//A sequencia nao confere com o senso
		s_match=0;
		
	if(prox->pasenso == atual->pasenso+1)//A sequencia confere com o antisenso
		as_match++;
	else//A sequencia nao confere com o antisenso
		as_match=0;
		
	return;	
}

__host__ __device__ void build_grafo(vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
		
	//Define cada vértice
	a->vertice = 'A';
	c->vertice = 'C';
	g->vertice = 'G';
	t->vertice = 'T';
	
	//Inicializa as marcações
	a->psenso = 0;
	a->pasenso = 0;
	c->psenso = 0;
	c->pasenso = 0;
	g->psenso = 0;
	g->pasenso = 0;
	t->psenso = 0;
	t->pasenso = 0;
	
	//Conecta os vértices
	a->a = a;
	a->c = c;
	a->g = g;
	a->t = t;
	
	c->a = a;
	c->c = c;
	c->g = g;
	c->t = t;
	
	g->a = a;
	g->c = c;
	g->g = g;
	g->t = t;
	
	t->a = a;
	t->c = c;
	t->g = g;
	t->t = t;
	
	return;
}

__global__ void set_grafo(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
	
	//Configura grafo
	int i;
	vgrafo *atual;
	
	build_grafo(a,c,g,t);
	
	i=0;
	//Configura sequência senso
	while(senso[i] != '\0'){
		i++;
		atual = busca_vertice(senso[i],a,c,g,t);
		atual->psenso=i;
	}
	
	i=0;
	//Configura sequência antisenso
	while(antisenso[i] != '\0'){
		i++;
		atual = busca_vertice(antisenso[i],a,c,g,t);
		atual->pasenso=i;
	}
		
	
	return;
}
