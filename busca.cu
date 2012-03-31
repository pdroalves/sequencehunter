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


////////////////////////////////////////////////////////////////////////////////////////
//////////////////					Kernel Principal 				////////////////////
////////////////////////////////////////////////////////////////////////////////////////
__global__ void k_busca(const int bloco1,const int bloco2,const int blocos,char **data,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
 
  ////////
  ////////
  ////////
  ////////		Recebe o tamanho dos blocos 1 e 2
  ////////		Recebe o tamanho total da sequência
  ////////		Recebe o endereço com todo o buffer
  ////////		Recebe ponteiros para cada vertice do grafo
  ////////
  ////////
  ////////
  ////////
  											
  const int posicao = blockIdx.x*blockDim.x + threadIdx.x;
  char *seq = data[posicao];//Seto ponteiro para a sequência que será analisada
  int i;
  int s_match;
  int as_match;
  vgrafo *atual;
  vgrafo *anterior;
  int x0;/////Essas variáveis guardam o intervalo onde podemos encontrar os elementos que queremos
  int xn;/////
  int size = bloco1 + bloco2;
  int blocoZ = blocos - size;//Total de bases que queremos encontrar
  char tipo;						
  s_match = as_match = 0;
  i=0;
  
  ////////////////////
  ////////////////////										
  //Iteração inicial//																			
  ////////////////////
  ////////////////////
  atual = busca_vertice(seq[0],a,c,g,t);
  if(atual != NULL)
    caminhar(NULL,atual,&s_match,&as_match);
  i++;
  anterior = atual;

  																			
  ///////////////////////
  ///////////////////////					
  //Iterações seguintes//																			
  ///////////////////////
  ///////////////////////
  										
#pragma unroll 1
  while( seq[i] != '\0' && s_match < size && as_match < size) {
	//  printf("s_match: %d\n",s_match);
	if(s_match == bloco1){
		//printf("Th: %d --> Bloco 1 encontrado na posicao %d, %c-> Sequência senso.\n",posicao,i,seq[i]);
		tipo = 'S';//Senso
		x0 = i;//Marca primeiro elemento 
		xn = x0 + blocoZ;//Marca primeiro elemento do bloco 2
		i = xn;  //Salta o bloco variável
	}
	if(as_match == bloco2){
	//	printf("Bloco 2 encontrado na posicao %d -> Sequência antisenso.\n",i);
		tipo = 'N';//Não-Senso
		x0 = i;//Marca primeiro elemento 
		xn = x0 + blocoZ;//Marca primeiro elemento do bloco 2
		i = xn;  //Salta o bloco variável
	}
    atual = busca_vertice(seq[i],a,c,g,t);
    if(atual != NULL)
      caminhar(anterior,atual,&s_match,&as_match);
    i++;
    anterior = atual;
  }

  ///////////////////////////////											
  //Guarda o que foi encontrado//
  ///////////////////////////////
  
  //printf("s_match: %d - as_match: %d\n",s_match,as_match);

	if(s_match == size){
	  seq[0] = tipo;
	  #pragma unroll 50
	  for(i=1;i<=blocoZ;i++){
		  seq[i] = seq[x0 + i-1];
		}
		seq[i] = '\0';
		//printf("%s\n",seq);
		//printf("%s\n\n",seq);
		return;
	}	
	
	if(as_match == size){
	  seq[0] = tipo;
	  #pragma unroll 50
	  for(i=1;i<=blocoZ;i++){
		  seq[i] = seq[x0 + i-1];
		}
		seq[i] = '\0';
		//printf("%s\n\n",seq);
		return;
	}
	
	seq[0] = '\0';
											
  return;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

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
  int j;
  int size;
  vgrafo *atual;
											
  for(size=0;senso[size] != '\0';size++);
											
  build_grafo(size,a,c,g,t);
											
  i=0;
  j=0;
  printf("Configurando senso.\n");
  //Configura sequência senso
  while(senso[i] != '\0'){
    atual = busca_vertice(senso[i],a,c,g,t);
    if(atual != NULL){
		atual->s_marcas[i-j]=1;
		//printf("%c marcado na posicao %d.\n",atual->vertice,i-j);
	}else{
		//printf("Elemento variável encontrado.\n");
		j++;
	}
    i++;
  }
											
  i=0;
  j=0;
  printf("\nConfigurando antisenso.\n");
  //Configura sequência antisenso
  while(antisenso[i] != '\0'){
    atual = busca_vertice(antisenso[i],a,c,g,t);
      if(atual != NULL){
		atual->as_marcas[i-j]=1;
		//printf("%c marcado na posicao %d.\n",atual->vertice,i-j);
	}else{
		//printf("Elemento variável encontrado.\n");
		j++;
	}
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
