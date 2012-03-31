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


#if defined(__CUDA_ARCH__) && (__CUDA_ARCH__ < 200)//Toma cuidado de não usar printf sem que a máquina suporte.
#define printf(f, ...) ((void)(f, __VA_ARGS__),0)
#endif


__global__ void set_grafo(char*,char *,vgrafo*,vgrafo*,vgrafo*, vgrafo*);
__host__ __device__ void caminhar(vgrafo*,vgrafo*, int*,int*);
__device__ void build_grafo(vgrafo*,vgrafo*,vgrafo*, vgrafo*);
__host__ __device__ vgrafo* busca_vertice(char,vgrafo *,vgrafo *,vgrafo *, vgrafo *);

__global__ void k_busca(int *matchs,char **data,const int size,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
											
  //int posicao = blockIdx.x*blockDim.x + threadIdx.x;
  int posicao = threadIdx.x;
  const char *seq = data[posicao];//Seto ponteiro para a sequência que será analisada
  int i;
  //int found = 0;//1 se encontrar uma seq
  int s_match;
  int as_match;
  vgrafo *atual;
  vgrafo *anterior;
												
  s_match = as_match = 0;
  i=0;
											
												
  atual = busca_vertice(seq[0],a,c,g,t);
  if(atual != NULL)
    caminhar(NULL,atual,&s_match,&as_match);
  i++;
  anterior = atual;
											
#pragma unroll 1
  while( seq[i] != '\0' && s_match < (size) && as_match < (size)){
    atual = busca_vertice(seq[i],a,c,g,t);
    if(atual != NULL)
      caminhar(anterior,atual,&s_match,&as_match);
    i++;
    anterior = atual;
  }
											
  //Marca o que foi encontrado
  //1 se for senso
  //2 se for antisenso
  //0 se não for nada
  matchs[posicao] = s_match / (size);
  //matchs[posicao] = 2*(as_match / (size-1));
											
  return;
}

__host__ __device__ vgrafo* busca_vertice(char base,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
  //Funcao temporária. Ficará aqui até eu pensar em algo melhor
  switch(base){
  case 'A':
    //	printf("Retornei A\n");
    return a;
  case 'C':
    //	printf("Retornei C\n");
    return c;
  case 'G':
    //	printf("Retornei G\n");
    return g;
  case 'T':
    //	printf("Retornei T\n");
    return t;
  }
											
  return NULL;
}

__host__ __device__ void caminhar(vgrafo* anterior,vgrafo *atual, int *s_match,int *as_match){ 
  //Recebe o vertice atual e o anterior
  //Recebe um contador de bases acertadas para a sequencia senso s_match
  //Recebe um contador de bases acertadas para a sequencia antisenso as_match
											
  //OTIMIZAR! Tem muitos IFs
											
  //printf("Analisando base %c. %d -> %d\n",atual->vertice,*s_match,atual->s_marcas[(*s_match)]);
  if(atual->s_marcas[(*s_match)] == 1){//Elemento e posição batem com o que queremos
    (*s_match)++;
    //printf("Match: %c\n",atual->vertice);
  }
  else{//Não bate
    if(anterior != NULL)
      if(anterior->vertice != atual->vertice)
	(*s_match)=0;
  }
												
  if(atual->as_marcas[(*as_match)] == 1)//Elemento e posição batem com o que queremos
    (*as_match)++;
  else{//Não bate
    if(anterior != NULL)
      if(anterior->vertice != atual->vertice)
	(*as_match)=0;
  }
  //printf("s_match: %d\n",*s_match);
  return;	
}

__host__ __device__ void build_grafo(int size,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
												
  int i;
											
  //Define cada vértice
  a->vertice = 'A';
  c->vertice = 'C';
  g->vertice = 'G';
  t->vertice = 'T';
											
  //Inicializa as marcações
  a->s_marcas = (int*)malloc(size*sizeof(int));
  c->s_marcas = (int*)malloc(size*sizeof(int));
  g->s_marcas = (int*)malloc(size*sizeof(int));
  t->s_marcas = (int*)malloc(size*sizeof(int));
													
  a->as_marcas = (int*)malloc(size*sizeof(int));
  c->as_marcas = (int*)malloc(size*sizeof(int));
  g->as_marcas = (int*)malloc(size*sizeof(int));
  t->as_marcas = (int*)malloc(size*sizeof(int));
											
  for(i=0;i<size;i++){
    a->s_marcas[i] = 0;
    c->s_marcas[i] = 0;
    g->s_marcas[i] = 0;
    t->s_marcas[i] = 0;
												
    a->as_marcas[i] = 0;
    c->as_marcas[i] = 0;
    g->as_marcas[i] = 0;
    t->as_marcas[i] = 0;
  }
											
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
  int size;
  vgrafo *atual;
											
  for(size=0;senso[size] != '\0';size++);
											
  build_grafo(size,a,c,g,t);
											
  i=0;
  //Configura sequência senso
  while(senso[i] != '\0'){
    atual = busca_vertice(senso[i],a,c,g,t);
    atual->s_marcas[i]=1;
    printf("%c marcado na posicao %d.\n",atual->vertice,i);
    i++;
  }
											
  i=0;
  //Configura sequência antisenso
  while(antisenso[i] != '\0'){
    atual = busca_vertice(antisenso[i],a,c,g,t);
    atual->as_marcas[i]=1;
    i++;
  }
  /*
    for(i=0;i<size;i++){
    printf("%c: %d -> %d\n",'A',i,a->s_marcas[i]);
    printf("%c: %d -> %d\n",'C',i,c->s_marcas[i]);
    printf("%c: %d -> %d\n",'G',i,g->s_marcas[i]);
    printf("%c: %d -> %d\n",'T',i,t->s_marcas[i]);
    }*/
  return;
}

void send_buffer(Buffer *b,int n){
											
  return;
}
