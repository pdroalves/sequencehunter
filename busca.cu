//      busca.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação do algoritmo de busca por grafos utilizando CUDA.
//
//		27/03/2012


#include <cuda.h>
#includa "estruturas.h"

__host__ __device__ void caminhar(vgrafo*, vgrafo*, int*,int*);

__global__ void k_busca(int *matchs,char **data,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
	
	int posicao = blockIdx.x*blockDim.x + threadIdx.x;
	char *seq = data[posicao];
	int i;
	int found = 0;//1 se encontrar uma seq
	int s_match = 0;
	int as_match = 0;
	vgrafo *atual;
	vgrafo *prox;
	
	#pragma unroll 1
	while(seq[i+1] != \0 && s_match < size-1 && as_match < size-1){
		atual = busca_vertice(seq[i],a,c,g,t);
		prox = busca_vertice(seq[i+1],a,c,g,t);
		caminhar(atual,prox,&s_match,&as_match);
	}
	
	//Marca o que foi encontrado
	//1 se for senso
	//2 se for antisenso
	//0 se não for nada
	matchs[posicao] = s_match / (size-1);
	matchs[posicao] = 2*(as_match / (size-1));
	
	return;
}

__host__ __device__ vgrafo* busca_vertice(char c,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
	//Funcao temporária. Ficará aqui até eu pensar em algo melhor
	
	switch(c){
		case "A":
			return a;
			break;
		case "C":
			return c;
			break;
		case "G":
			return g;
			break;
		case "T":
			return t;
			break;
	}
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
