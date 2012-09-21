//      aux.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include "cuda.h"
#include "cuda_runtime_api.h"
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "log.h"
#include "pilha.h"

#define buffer_size 1024 //Capacidade máxima do buffer
#define LIMITE_PILHA 10000
const char tmp_s_name[11] = "tmp_sensos";
const char tmp_as_name[15] = "tmp_antisensos";
__constant__ char *d_buffer[buffer_size];
int buffer_flag;//0 se o buffer já foi carregado, 1 se estiver sendo carregado.
gboolean verbose;
gboolean silent;
gboolean memory_save;

void auxCUDA(char *c,const int bloco1,const int bloco2,const int blocos,pilha *p_sensos,pilha *p_antisensos);
void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,pilha *p_sensos,pilha *p_antisensos);
void setup_for_cuda(char*,vgrafo*,vgrafo*,vgrafo*, vgrafo*);
void setup_without_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t);
void load_buffer_CUDA(Buffer *b,char** s,int n);
void load_buffer_NONCuda(Buffer *b,int n);
void cudaIteracoes(int bloco1,int bloco2,int blocoV,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t,pilha *p_senso,pilha *p_antisenso);
void NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t,pilha *p_sensos,pilha *p_antisensos);
	
void aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos,pilha *p_sensos,pilha *p_antisensos,gboolean disable_cuda,gboolean sil,gboolean verb,gboolean mem_save){
  verbose = verb;
  sil = silent;
  mem_save = memory_save;
  if(CUDA){}
  //auxCUDA(c,bloco1,bloco2,blocos,p_sensos,p_antisensos);
  else
    auxNONcuda(c,bloco1,bloco2,blocos,p_sensos,p_antisensos);
  return;
}

void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,pilha *p_sensos,pilha *p_antisensos){
	
  int n;//Elementos por sequência
  vgrafo g_a;
  vgrafo g_c;
  vgrafo g_g;
  vgrafo g_t;
  //Arrumar nova maneira de contar o tempo sem usar a cuda.h
  //cudaEvent_t start;
  //cudaEvent_t stop;
  //cudaEventCreate(&start);
  //cudaEventCreate(&stop);
  float tempo = 0;
  printf("OpenMP Mode.\n");
  get_setup(&n);
	
  setup_without_cuda(c,&g_a,&g_c,&g_g,&g_t);
	
  printString("Dados inicializados.\n",NULL);
  printSet(n);
  printString("Iniciando iterações:\n",NULL);
	
  //cudaEventRecord(start,0);
  NONcudaIteracoes(bloco1,bloco2,blocos,n,&g_a,&g_c,&g_g,&g_t,p_sensos,p_antisensos);
  //cudaEventRecord(stop,0);
  //cudaEventSynchronize(stop);
  //cudaEventElapsedTime(&tempo,start,stop);
    
  printString("Iterações terminadas. Tempo: ",NULL);
  print_tempo(tempo);
  destroy_grafo(&g_a,&g_c,&g_g,&g_t);
	
  return;	
}
	
/*void auxCUDA(char *c,const int bloco1,const int bloco2,const int blocos,pilha *p_sensos,pilha *p_antisensos){
  printf("CUDA Mode.\n");
  int n;//Elementos por sequência
  vgrafo *d_a;
  vgrafo *d_c;
  vgrafo *d_g;
  vgrafo *d_t;
  cudaEvent_t start;
  cudaEvent_t stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  float tempo;
	
  get_setup(&n);
	
  cudaMalloc((void**)&d_a,sizeof(vgrafo));
  cudaMalloc((void**)&d_c,sizeof(vgrafo));
  cudaMalloc((void**)&d_g,sizeof(vgrafo));
  cudaMalloc((void**)&d_t,sizeof(vgrafo));
    
  //Inicializa
  setup_for_cuda(c,d_a,d_c,d_g,d_t);
	
  printString("Dados inicializados.\n",NULL);
  printSet(n);
  printString("Iniciando iterações:\n",NULL);
	
  // cudaEventRecord(start,0);
  cudaIteracoes(bloco1,bloco2,blocos,n,d_a,d_c,d_g,d_t,p_sensos,p_antisensos);
  // cudaEventRecord(stop,0);
  // cudaEventSynchronize(stop);
  // cudaEventElapsedTime(&tempo,start,stop);
    
  printString("Iterações terminadas. Tempo: ",NULL);
  print_tempo(tempo);
  cudaFree(d_a);
  cudaFree(d_c);
  cudaFree(d_g);
  cudaFree(d_t);
  cudaThreadExit();
  return;

  }
  void setup_for_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t){
  //Recebe um vetor de caracteres com o padrão a ser procurado
  //Recebe ponteiros para os quatro vértices do grafo já na memória da GPU
  char *d_senso;
  char *d_antisenso;
  int size = strlen(seq)+1;
	
  //Aloca memória na GPU
  cudaMalloc((void**)&d_senso,size*sizeof(char));
  cudaMalloc((void**)&d_antisenso,size*sizeof(char));
    
  cudaMemcpy(d_senso,seq,size*sizeof(char),cudaMemcpyHostToDevice);
  cudaMemcpy(d_antisenso,(const void*)get_antisenso(seq),size*sizeof(char),cudaMemcpyHostToDevice);
    
  //Configura grafos direto na memória da GPU
  set_grafo_helper(d_senso,d_antisenso,d_a,d_c,d_g,d_t);
  printString("Grafo de busca contigurado.",NULL);
  cudaFree(d_senso);
  cudaFree(d_antisenso);
  return;
  }*/

void setup_without_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t){
  //Recebe um vetor de caracteres com o padrão a ser procurado
  //Recebe ponteiros para os quatro vértices do grafo

  //Configura grafo
  char* hold = get_antisenso(seq);
  set_grafo_NONCuda(seq,hold,d_a,d_c,d_g,d_t);
  printString("Grafo de busca contigurado.",NULL);
  free(hold);
  return;
}

/*void load_buffer_CUDA(Buffer *b,char** s,int n){
  int i;
	
  if(b->load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
  fill_buffer(b,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
  if(b->load != -1){
  print_seqs_carregadas(b->load);
  //printf("%s\n",b->seq[0]);
  for(i=0;i<b->load;i++)
  cudaMemcpy(d_buffer[i],b->seq[i],(n+1)*sizeof(char),cudaMemcpyHostToDevice);
			
  cudaMemcpy(s,d_buffer,b->load*sizeof(char*),cudaMemcpyHostToDevice);
  }
  //////////////////////////////////
  buffer_flag = 0;//Sinal aberto////
  //////////////////////////////////
			
  }
		
	
  return;
  }
*/

void load_buffer_NONCuda(Buffer *b,int n){

  if(b->load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
    fill_buffer(b,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
    //////////////////////////////////
    buffer_flag = 0;//Sinal aberto////
    //////////////////////////////////		
  }
		
	
  return;
}

void NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t,pilha *p_sensos,pilha *p_antisensos){
	
  Buffer buffer;
  char **s;
  char *tmp;
  int blocoV = blocos - bloco1 - bloco2+1;
  int iter;
  int buffer_size_NC = 5120;
  int i;
  int tam;
  int razao;
  int p=0;
  pilha *novo;
  FILE *tmp_sensos;
  FILE *tmp_antisensos;
		  
	
  //Inicializa buffer
  prepare_buffer(&buffer,buffer_size_NC);

  //tmp = (char*)malloc(blocoV*sizeof(char));
    
			
#pragma omp parallel num_threads(3) shared(buffer) shared(buffer_flag) shared(p_sensos) shared(p_antisensos) shared(iter) shared(tmp_sensos) shared(tmp_antisensos)
  {	
		
#pragma omp sections
    {
#pragma omp section
      {
	while(buffer.load != -1){//Looping até o final do buffer
	  ///////////////////////////////////
	  buffer_flag = 1;//Sinal fechado////
	  ///////////////////////////////////	
	  load_buffer_NONCuda(&buffer,n);
	  while(buffer.load > 0){
	  }
	}
	///////////////////////////////////
	buffer_flag = 0;//Sinal Aberto////
	///////////////////////////////////	
	if(!memory_save){
		///////////////////////////////////
		buffer_flag = 1;//Sinal fechado////
		///////////////////////////////////	
		while(tamanho_da_pilha(p_sensos) > 0){
		  despejar_seq(desempilha(p_sensos),tmp_sensos);
		}
		while(tamanho_da_pilha(p_antisensos) > 0){
		  despejar_seq(desempilha(p_antisensos),tmp_antisensos);
		///////////////////////////////////
		buffer_flag = 0;//Sinal aberto////
		///////////////////////////////////	
	}
	}
      }
#pragma omp section
      {

	if(memory_save){	  
		while( buffer_flag == 1){
		}//Aguarda para que o buffer seja enchido pela primeira vez
			

		tmp_sensos = fopen(tmp_s_name,"w");
		tmp_antisensos = fopen(tmp_as_name,"w");
		while(buffer.load != -1){
		  ///////////////////////////////////
		  buffer_flag = 1;//Sinal fechado////
				  ///////////////////////////////////	
		  if(tamanho_da_pilha(p_sensos) > LIMITE_PILHA){
			despejar_seq(desempilha(p_sensos),tmp_sensos);
		  }
		  if(tamanho_da_pilha(p_antisensos) > LIMITE_PILHA){
			despejar_seq(desempilha(p_antisensos),tmp_antisensos);
		  }
		  ///////////////////////////////////
		  buffer_flag = 0;//Sinal aberto////
		  ///////////////////////////////////	
		}

		///////////////////////////////////
		buffer_flag = 1;//Sinal fechado////
		///////////////////////////////////	
		while(tamanho_da_pilha(p_sensos) > 0){
		  despejar_seq(desempilha(p_sensos),tmp_sensos);
		}
		while(tamanho_da_pilha(p_antisensos) > 0){
		  despejar_seq(desempilha(p_antisensos),tmp_antisensos);
		}
		///////////////////////////////////
		buffer_flag = 0;//Sinal aberto////
				///////////////////////////////////	
		  
		fclose(tmp_sensos);
		fclose(tmp_antisensos);	
		}
      }
#pragma omp section
      {
	while( buffer_flag == 1){
	}//Aguarda para que o buffer seja enchido pela primeira vez
			
	while(buffer.load != -1){
	  //Realiza loop enquanto existirem sequências para encher o buffer	
	  busca(bloco1,bloco2,blocos,&buffer,0,1,d_a,d_c,d_g,d_t);//Kernel de busca
					
	  tam = buffer.load;
	  p += tam;
	  //printf("%d\n",p);
	  for(i = 0; i < tam;i++){//Copia sequências senso e antisenso encontradas
	    switch(buffer.resultado[i]){
	    case 1:
	      tmp = buffer.seq[i];
	      if(verbose == TRUE && silent != TRUE)	
		printf("S: %s - %d\n",tmp,p);
	      novo = criar_elemento_pilha(tmp);
	      while(buffer_flag == 1){ }//Se a pilha estiver sendo esvaziada, aguarda
	      empilha(p_sensos,novo);
	      //printString("Senso:",tmp);
	      buffer.load--;
	      break;
	    case 2:
	      tmp = buffer.seq[i];
	      if(verbose == TRUE && silent != TRUE)
		printf("N: %s - %d\n",tmp,p);
	      novo = criar_elemento_pilha((char*)get_antisenso(tmp));
	      while(buffer_flag == 1){ }//Se a pilha estiver sendo esvaziada, aguarda
	      empilha(p_antisensos,novo);
	      //printString("Antisenso:",tmp);
	      buffer.load--;
	      break;
	    default:
	      buffer.load--;
	      break;
	    }
	  }
					
										
	  if(buffer.load != 0)
	    {
	      printf("Erro! Buffer não foi totalmente esvaziado.\n");
	      buffer.load = 0;
	    }
		
	  while(buffer_flag == 1 || buffer.load == 0){
	  }//Espera o buffer ser carregado
				
	}
      }
		
    }
  }

  //printf("Iterações executadas: %d.\n",iter);
  //free(tmp);
  return;
}

/*void cudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t,pilha *p_sensos,pilha *p_antisensos){
	
  Buffer buffer;
  char **s;
  char *tmp;
  int i;
  int blocoV = blocos - bloco1 - bloco2+1;

  //Inicializa buffer
  prepare_buffer(&buffer,buffer_size);

  //Aloca memória
  for(i=0;i<buffer_size;i++)
  cudaMalloc((void**)&d_buffer[i],(n+2)*sizeof(char));//Aloco n+1 posicoes para as bases e +1 para processamento interno no arquivo cuda_stack.cu
  cudaMalloc((void**)&s,buffer_size*sizeof(char*));
  tmp = (char*)malloc(blocoV*sizeof(char));
    
			
  #pragma omp parallel num_threads(2) shared(buffer) shared(buffer_flag) shared(p_sensos) shared(p_antisensos)
  {	
  #pragma omp master
  {
  while(buffer.load != -1){//Looping até o final do buffer
  //printf("%d.\n",buffer.load);
  ///////////////////////////////////
  buffer_flag = 1;//Sinal fechado////
  ///////////////////////////////////	
  load_buffer_CUDA(&buffer,s,n);
  while(buffer.load > 0){
  }
  }
  ///////////////////////////////////
  buffer_flag = 0;//Sinal Aberto////
  ///////////////////////////////////	
			
  }
		
  #pragma omp single		
  {
  int num_threads;
  int num_blocks=1;
  const char *error;
  while( buffer_flag == 1){
  }//Aguarda para que o buffer seja enchido pela primeira vez
			
  while( buffer.load != -1){
  //Realiza loop enquanto existirem sequências para encher o buffer		
		
  num_threads = buffer_size>=buffer.load?buffer.load:buffer_size;
					
  k_busca_helper(num_blocks,num_threads,bloco1,bloco2,blocos,s,d_a,d_c,d_g,d_t);//Kernel de busca
  error = cudaGetErrorString(cudaGetLastError());
  if(strcmp(error,"no error") != 0)
  printf("%s\n",error);
  for(i=0;i<num_threads;i++){//Copia sequências senso e antisenso encontradas
  cudaMemcpy(tmp,d_buffer[i],sizeof(char),cudaMemcpyDeviceToHost);
						
  switch(tmp[0]){
  case 'S':
  cudaMemcpy(tmp,d_buffer[i]+1,blocoV*sizeof(char),cudaMemcpyDeviceToHost);
  //printf("S: %s\n",tmp);
  empilha(p_sensos,criar_elemento_pilha(tmp));
  break;
  case 'N':
  cudaMemcpy(tmp,d_buffer[i]+1,blocoV*sizeof(char),cudaMemcpyDeviceToHost);
  //printf("N: %s\n",tmp);
  empilha(p_antisensos,criar_elemento_pilha((char*)get_antisenso(tmp)));
  break;
  default:
  break;
  }
  }
  buffer.load = 0;//Avisando sobre buffer vazio
		
  while(buffer_flag == 1 || buffer.load == 0){
  }//Espera o buffer ser carregado
				
  }
  }
  }
  return;
  }*/


