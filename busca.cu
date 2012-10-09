//      busca.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação do algoritmo de busca por grafos.
//
//		27/03/2012

#include <stdio.h>
#include <cuda.h>
extern "C" {
#include "estruturas.h"
}

#if defined(__CUDA_ARCH__) && (__CUDA_ARCH__ < 200)//Toma cuidado de não usar printf sem que a máquina suporte.
#define printf(f, ...) ((void)(f, __VA_ARGS__),0)
#endif

extern "C" __host__ __device__ void caminhar(vgrafo*,vgrafo*,vgrafo*, int*,int*);
extern "C" __host__ __device__ vgrafo* busca_vertice(char,vgrafo *,vgrafo *,vgrafo *, vgrafo *);
__global__ void helloCUDA(float f)
{
  printf("Hello thread %d, block %d f=%f\n", threadIdx.x,blockIdx.x, f);
}
////////////////////////////////////////////////////////////////////////////////////////
//////////////////					Buscador		 				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
///////////////				Metodo de busca com CUDA				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

__global__ void k_buscador(int bloco1,int bloco2, int blocos,char **data,int *resultados,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){

	
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
  int size = bloco1 + bloco2;
  int blocoZ;//Total de bases que queremos encontrar
  int i;
  int s_match;
  int as_match;
  vgrafo *atual;
  vgrafo *anterior;
  vgrafo *ant_anterior;
  int x0;/////Essas variáveis guardam o intervalo onde podemos encontrar os elementos que queremos
  int x0S;
  int x0A;
  int totalmatchs;
  char *seq;
  int id;
  int tipo;
  
  blocoZ = blocos - size;
  x0 = 1;
  x0S = 1;
  x0A = 1;
  totalmatchs = blocos;
  id = threadIdx.x;
  seq = data[id];
  s_match = as_match = 0;
  tipo = 0;
  i=0;
  ////////////////////
  ////////////////////										
  //Iteração inicial//																			
  ////////////////////
  ////////////////////
  if(0 == bloco1) x0S = i;
  if(0 == bloco2) x0A = i;
  ant_anterior = busca_vertice(seq[i],a,c,g,t);
  if(ant_anterior != NULL){
    caminhar(NULL,NULL,ant_anterior,&s_match,&as_match);
    i++;
  }
		
  if(s_match == bloco1) x0S = i;
  if(as_match == bloco2) x0A = i;
  anterior = busca_vertice(seq[i],a,c,g,t);
  caminhar(NULL,ant_anterior,anterior,&s_match,&as_match);
  i++;
	  																	
  ///////////////////////
  ///////////////////////					
  //Iterações seguintes//																			
  ///////////////////////
  ///////////////////////
						
  while( seq[i] != '\0' && s_match < totalmatchs && as_match < totalmatchs) {
    //printf("%d - s_match: %d\n",i+1,s_match);
    //printf("%d - as_match: %d\n",i+1,as_match);
		  
    if(s_match == bloco1){
      //printf("Th: %d --> Bloco 1 encontrado na posicao %d, %s-> Sequência senso.\n",posicao,i,seq);
      x0S = i;
    }
    if(as_match == bloco2){
      //printf("Th: %d --> Bloco 2 encontrado na posicao %d, %s-> Sequência antisenso.\n",posicao,i,seq);
      x0A = i;
    }
    atual = busca_vertice(seq[i],a,c,g,t);
    if(atual != NULL)
      caminhar(ant_anterior,anterior,atual,&s_match,&as_match);
    i++;
    ant_anterior = anterior;
    anterior = atual;
  }
  ///////////////////////////////											
  //Guarda o que foi encontrado//
  ///////////////////////////////
	  
 // printf("s_match: %d - as_match: %d\n",s_match,as_match);
 // printf("totalmatchs: %d\n",totalmatchs);
  
  if(s_match == totalmatchs){
    x0 = x0S;
    tipo = 1;
  }
  if(as_match == totalmatchs){
    x0 = x0A;
    tipo = 2;
  }
	
  resultados[id] = tipo;

  //printf("%s -> s_match= %d e as_match=%d\n",seq,s_match,as_match);
  if(s_match == totalmatchs || as_match == totalmatchs){
		for(i=0;i<blocoZ;i++){
		  seq[i] = seq[x0 + i];
		}
		seq[i] = '\0';
	}
	
	return;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
///////////////				Metodo de busca sem CUDA				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

extern "C" __host__ void buscador(const int bloco1,const int bloco2,const int blocos,Buffer *buffer,int *resultados,int id,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
 
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
  int size = bloco1 + bloco2;
  int blocoZ = blocos - size;//Total de bases que queremos encontrar
  int i;
  int s_match;
  int as_match;
  vgrafo *atual;
  vgrafo *anterior;
  vgrafo *ant_anterior;
  int x0=1;/////Essas variáveis guardam o intervalo onde podemos encontrar os elementos que queremos
  int x0S=1;
  int x0A=1;
  int totalmatchs = blocos;
  s_match = as_match = 0;
  int tipo = 0;
  char *seq;
  seq = buffer->seq[id];
  i=0;
	  
  ////////////////////
  ////////////////////										
  //Iteração inicial//																			
  ////////////////////
  ////////////////////
  if(s_match == bloco1) x0S = i;
  if(as_match == bloco2) x0A = i;
  ant_anterior = busca_vertice(seq[i],a,c,g,t);
  if(ant_anterior != NULL){
    caminhar(NULL,NULL,ant_anterior,&s_match,&as_match);
    i++;
  }
		
  if(s_match == bloco1) x0S = i;
  if(as_match == bloco2) x0A = i;
  anterior = busca_vertice(seq[i],a,c,g,t);
  caminhar(NULL,ant_anterior,anterior,&s_match,&as_match);
  i++;
	  
																				
  ///////////////////////
  ///////////////////////					
  //Iterações seguintes//																			
  ///////////////////////
  ///////////////////////
						
  while( seq[i] != '\0' && s_match < totalmatchs && as_match < totalmatchs) {
    //printf("s_match: %d\n",s_match);
    //printf("as_match: %d\n",as_match);
		  
    if(s_match == bloco1){
      //printf("Th: %d --> Bloco 1 encontrado na posicao %d, %s-> Sequência senso.\n",posicao,i,seq);
      x0S = i;
    }
    if(as_match == bloco2){
      //printf("Th: %d --> Bloco 2 encontrado na posicao %d, %s-> Sequência antisenso.\n",posicao,i,seq);
      x0A = i;
    }
    atual = busca_vertice(seq[i],a,c,g,t);
    if(atual != NULL)
      caminhar(ant_anterior,anterior,atual,&s_match,&as_match);
    i++;
    ant_anterior = anterior;
    anterior = atual;
  }

  ///////////////////////////////											
  //Guarda o que foi encontrado//
  ///////////////////////////////
	  
  //printf("s_match: %d - as_match: %d\n",s_match,as_match);

  if(s_match == totalmatchs){
    x0 = x0S;
    tipo = 1;
  }
  if(as_match == totalmatchs){
    x0 = x0A;
    tipo = 2;
  }
	
  resultados[id] = tipo;

  if(s_match == totalmatchs || as_match == totalmatchs){
    //printf("%s -> s_match= %d e as_match=%d\n",seq,s_match,as_match);
    for(i=0;i<blocoZ;i++){
      seq[i] = seq[x0 + i];
    }
    seq[i] = '\0';
  }
	

	
return;
}
////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   	Auxiliar     ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
extern "C" void checkCudaError(){
	char erro[100];
	strcpy(erro,cudaGetErrorString(cudaGetLastError()));
    if(strcmp(erro,"no error") != 0){
		printf("%s\n",erro);
		exit(1);
    }   
}

extern "C" void k_busca(int num_blocks,int num_threads,const int bloco1,const int bloco2,const int blocos,char **data,int *resultados,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t){
	dim3 dimBlock(num_threads);
	dim3 dimGrid(num_blocks);
	
	k_buscador<<<dimGrid,dimBlock>>>(bloco1,bloco2,blocos,data,resultados,d_a,d_c,d_g,d_t);//Kernel de busca
	//k_buscador<<<dimGrid,dimBlock>>>();//Kernel de busca
	
	//helloCUDA<<<dimGrid,dimBlock>>>(5);
	checkCudaError();
	return;
}

extern "C" void busca(const int bloco1,const int bloco2,const int blocos,Buffer *buffer,int *resultados,vgrafo *h_a,vgrafo *h_c,vgrafo *h_g,vgrafo *h_t){
	int i;
	int size;
	
	size = buffer->load;
	
	for(i=0; i < size; i++)
		buscador(bloco1,bloco2,blocos,buffer,resultados,i,h_a,h_c,h_g,h_t);//Metodo de busca
		
	return;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


extern "C" __host__ __device__ vgrafo* busca_vertice(char base,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
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

extern "C" __host__ __device__ void caminhar(vgrafo *ant_anterior,vgrafo* anterior,vgrafo *atual, int *s_match,int *as_match){ 
  //Recebe o vertice atual e o anterior
  //Recebe um contador de bases acertadas para a sequencia senso s_match
  //Recebe um contador de bases acertadas para a sequencia antisenso as_match
											
  //OTIMIZAR! Tem muitos IFs
											
  //printf("Analisando base %c. %d -> %d\n",atual->vertice,*s_match,atual->s_marcas[(*s_match)]);
  if(atual->s_marcas[(*s_match)] == 1)//Elemento e posição batem com o que queremos
    (*s_match)++;
  else{//Não bate
    if(anterior != NULL && ant_anterior != NULL)
      if(anterior->vertice != atual->vertice || anterior->vertice != ant_anterior->vertice)
	(*s_match)=0;
  }
												
  if(atual->as_marcas[(*as_match)] == 1)//Elemento e posição batem com o que queremos
    (*as_match)++;
  else{//Não bate
    if(anterior != NULL && ant_anterior != NULL)
      if(anterior->vertice != atual->vertice || anterior->vertice != ant_anterior->vertice)
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

extern "C" __global__ void set_grafo(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
											
  //Configura grafo
  int i;
  int size;
  vgrafo *atual;
											
  for(size=0;senso[size] != '\0';size++);//Pega tamanho das sequências
  size++;
  build_grafo(size,a,c,g,t);
											
  i=0;
  printf("Configurando senso. -> %s.\n",senso);
  //Configura sequência senso
  while(senso[i] != '\0'){
    atual = busca_vertice(senso[i],a,c,g,t);
    if(atual != NULL){
      atual->s_marcas[i]=1;
      printf("%c marcado na posicao %d.\n",atual->vertice,i);
     }else{
      //printf("Elemento variável encontrado.\n");
      a->s_marcas[i]=1;
      c->s_marcas[i]=1;
      g->s_marcas[i]=1;
      t->s_marcas[i]=1;
    }
    i++;
  }
											
  i=0;
  printf("\nConfigurando antisenso. -> %s.\n",antisenso);
  //Configura sequência antisenso
  while(antisenso[i] != '\0'){
    atual = busca_vertice(antisenso[i],a,c,g,t);
    if(atual != NULL){
      atual->as_marcas[i]=1;
      printf("%c marcado na posicao %d.\n",atual->vertice,i);
      }else{
      //printf("Elemento variável encontrado.\n");
      a->as_marcas[i]=1;
      c->as_marcas[i]=1;
      g->as_marcas[i]=1;
      t->as_marcas[i]=1;
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


extern "C" void set_grafo_helper(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
  set_grafo<<<1,1>>>(senso,antisenso,a,c,g,t);
}

extern "C" void set_grafo_NONCuda(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
											
  //Configura grafo
  int i;
  int size;
  vgrafo *atual;
											
  size = strlen(senso);//Pega tamanho das sequências
  build_grafo(size,a,c,g,t);
											
  i=0;
  printf("Configurando senso. -> %s.\n",senso);
  //Configura sequência senso
  while(senso[i] != '\0'){
    atual = busca_vertice(senso[i],a,c,g,t);
    if(atual != NULL){
      atual->s_marcas[i]=1;
      printf("%c marcado na posicao %d.\n",atual->vertice,i);
    }else{
      //printf("Elemento variável encontrado.\n");
      a->s_marcas[i]=1;
      c->s_marcas[i]=1;
      g->s_marcas[i]=1;
      t->s_marcas[i]=1;
    }
    i++;
  }
											
  i=0;
  printf("\nConfigurando antisenso. -> %s.\n",antisenso);
  //Configura sequência antisenso
  while(antisenso[i] != '\0'){
    atual = busca_vertice(antisenso[i],a,c,g,t);
    if(atual != NULL){
      atual->as_marcas[i]=1;
      printf("%c marcado na posicao %d.\n",atual->vertice,i);
    }else{
      //printf("Elemento variável encontrado.\n");
      a->as_marcas[i]=1;
      c->as_marcas[i]=1;
      g->as_marcas[i]=1;
      t->as_marcas[i]=1;
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


extern "C" void destroy_grafo(vgrafo *a,vgrafo *c,vgrafo *g,vgrafo *t){
  free(a->s_marcas);
  free(a->as_marcas);
  //free(a);
  free(c->s_marcas);
  free(c->as_marcas);
  //free(c);
  free(g->s_marcas);
  free(g->as_marcas);
  //free(g);
  free(t->s_marcas);
  free(t->as_marcas);
  //free(t);
  return;
}

extern "C" void destroy_grafo_CUDA(int size,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t){
  cudaFree(a);
  cudaFree(c);
  cudaFree(g);
  cudaFree(t);
  return;
}
