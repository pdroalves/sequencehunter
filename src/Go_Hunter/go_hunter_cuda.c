#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#define SLEEP(a) Sleep(1000*a)
#else
#define SLEEP(a) sleep(a)
#endif
#include "../Headers/database_manager.h"
#include "../Headers/database.h"
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"
#include "../Headers/operacoes.h"
#include "../Headers/busca.h"
#include "../Headers/log.h"
#include "../Headers/go_hunter.h"
#include "../Headers/fila.h"
#include "../Headers/socket.h"
#include "sqlite3.h"

#define buffer_size 1024 // Capacidade máxima do buffer
#define LOADER_QUEUE_MAX_SIZE 1e6
#define GUI_SOCKET_PORT 9332
#define GIGA 1073741824 

omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
gboolean debug;
gboolean central_cut;
gboolean regiao_5l;
gboolean gui_run;
int dist_regiao_5l;
int tam_regiao_5l;
int sent_to_db;
int fsenso;
int fasenso;	
char **data;
short int *h_vertexes;
short int *h_candidates;
short int *d_vertexes;
short int *d_candidates;
int processadas;
int bytes_read = 0;
Socket *gui_socket;


// Lista de threads a serem criados
//
enum threads { 
  THREAD_SEARCH,
  THREAD_QUEUE,
  THREAD_DATABASE,
  OMP_NTHREADS
};
gboolean THREAD_DONE[OMP_NTHREADS];



int load_buffer_CUDA(char **h_seqs,int seq_size)
{
  //Enche o buffer e guarda a quantidade de sequencias carregadas.
  int loaded;
  int n;
  int i;
  fill_buffer(h_seqs,buffer_size,&loaded);	
  n = strlen(h_seqs[0]);
  for(i=0;i<loaded;i++){    
    cuda_convert_to_graph(h_seqs[i],n,&h_vertexes[i*n]);
    if( h_seqs[i][0]*(2+h_seqs[i][1]) != h_vertexes[i*n]){
      printf("Erro na %d - %d %s\n",loaded,h_vertexes[i*n],h_seqs[i]);
      exit(1);
    }
  }
  return loaded;
}


// Esse metodo eh bom para pre-carregar os dados, mas parece nao ser muito util
/*void loader_data_queue(
  Fila *loaded_data_queue,
  int seq_size )
  {
	
  ////////////////////////////////////////// 		
  // Carrega o buffer //////////////////////
  //////////////////////////////////////////
  int i;
  int loaded ;
  //////////////////////////////////////////
  // Inicializa
  //////////////////////////////////////////
  loaded = 0;
  
  data = (char**)malloc(buffer_size*sizeof(char*));
  for(i=0;i<buffer_size;i++)
  data[i] = (char*)malloc((seq_size+1)*sizeof(char));
	
  //////////////////////////////////////////
  // Loop que mantem fila cheia
  //////////////////////////////////////////
  while(loaded != GATHERING_DONE){
  if(tamanho_da_fila(loaded_data_queue) < 0.5*LOADER_QUEUE_MAX_SIZE){
  while(tamanho_da_fila(loaded_data_queue) < LOADER_QUEUE_MAX_SIZE)
  {
  loaded = load_buffer_CUDA(data, seq_size);

  // Enfileira tudo
  for(i=0; i < loaded; i++)
  enfileirar(loaded_data_queue,data[i],NULL,NULL);

  // Precisa garantir que nada seja sobrescrito
  for(i=0;i<buffer_size;i++)
  data[i] = (char*)malloc((seq_size+1)*sizeof(char));
  }
  }
  } 	
	  
  free(data);
  THREAD_DONE[THREAD_LOADER] = TRUE;
  return;
  }*/


/*void buffer_manager(	int *buffer_load,
  int seq_size,
  cudaStream_t stream )
  {
		
  int i;
  int loaded;
  //////////////////////////////////////////
  // Inicializa
  //////////////////////////////////////////
  data = (char**)malloc(buffer_size*sizeof(char*));
  for(i=0;i<buffer_size;i++)
  data[i] = (char*)malloc((seq_size+1)*sizeof(char));
  h_vertexes = (short int*)malloc(buffer_size*seq_size*sizeof(short int));
  //cudaHostAlloc((void**)&h_vertexes,buffer_size*seq_size*sizeof(short int),cudaHostAllocWriteCombined);
  h_candidates = (short int*)malloc(buffer_size*seq_size*sizeof(short int));
  //d_vertexes = (short int*)malloc(seq_size*sizeof(short int));
  cudaMalloc((void**)&d_vertexes,buffer_size*seq_size*sizeof(short int));
  cudaMalloc((void**)&d_candidates,buffer_size*seq_size*sizeof(short int));
    
  //////////////////////////////////////////
  // Carrega o buffer //////////////////////
  //////////////////////////////////////////
  while(*buffer_load != GATHERING_DONE){//Looping até o final do buffer
  if(*buffer_load == 0){      
  loaded = load_buffer_CUDA(data,seq_size);
  //      cudaHostGetDevicePointer(&d_vertexes,h_vertexes,0);
  cudaMemcpyAsync(d_vertexes,h_vertexes,loaded*seq_size*sizeof(short int),cudaMemcpyHostToDevice,stream);
  *buffer_load = loaded;
  }
  }
  THREAD_DONE[THREAD_BUFFER_LOADER] = TRUE;
  //////////////////////////////////////////
  //////////////////////////////////////////
  //////////////////////////////////////////	
  }*/

void search_manager(int *buffer_load,
		    Fila *toStore,
		    const int seqSize_an,
		    const int seqSize_bu,
		    int bloco1,
		    int bloco2,
		    int blocoV)
{	
  int i;
  short int *h_resultados;
  short int *d_resultados;
  short int *h_search_gaps;
  short int *d_search_gaps;
  char *central;
  char *cincol;
  int loaded;
  int hold;
  int p;
  int last_p;
  int gap;
  char **local_data;
  Event *hold_event;
  gboolean retorno;
  cudaStream_t stream;
  double sec;
  struct timeval start_time, end_time;
  fsenso=fasenso=0;
  
  //////////////////////////////////////////
  // Inicializa
  //////////////////////////////////////////
  cudaStreamCreate(&stream);
  data = (char**)malloc(buffer_size*sizeof(char*));
  for(i=0;i<buffer_size;i++)
    data[i] = (char*)malloc((seqSize_an+1)*sizeof(char));
  h_vertexes = (short int*)malloc(buffer_size*seqSize_an*sizeof(short int));
  //cudaHostAlloc((void**)&h_vertexes,buffer_size*seqSize_an*sizeof(short int),cudaHostAllocWriteCombined);
  h_candidates = (short int*)malloc(buffer_size*seqSize_an*sizeof(short int));
  //d_vertexes = (short int*)malloc(seqSize_an*sizeof(short int));
  cudaMalloc((void**)&d_vertexes,buffer_size*seqSize_an*sizeof(short int));
  cudaMalloc((void**)&d_candidates,buffer_size*seqSize_an*sizeof(short int));

  // Alloc: resultados de cada iteracao
  h_resultados = (short int*)malloc(buffer_size*sizeof(short int));
  h_search_gaps = (short int*)malloc(buffer_size*sizeof(short int));
  cudaMalloc((void**)&d_resultados,buffer_size*sizeof(short int));
  cudaMalloc((void**)&d_search_gaps,buffer_size*sizeof(short int));


  processadas = 0;

  *buffer_load = load_buffer_CUDA(data,seqSize_an);
  //cudaHostGetDevicePointer(&d_vertexes,h_vertexes,0);
  cudaMemcpyAsync(d_vertexes,h_vertexes,*buffer_load*seqSize_an*sizeof(short int),cudaMemcpyHostToDevice,stream);
 checkCudaError();
  gettimeofday(&start_time, NULL);
  while( *buffer_load != GATHERING_DONE){
    //Realiza loop enquanto existirem sequencias para encher o buffer
    
    loaded = *buffer_load;
    // Execuca iteracao
    k_busca(*buffer_load,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,d_vertexes,d_candidates,d_resultados,d_search_gaps,&stream);//Kernel de busca
		
    // Inicia processamento dos resultados
    processadas += loaded;
    cudaStreamSynchronize(stream);
	
    // Em casos reais, cada iteracao possuirah poucos eventos. Portanto, a copia de dados para 
    // o host serah bastante casual e esse trecho nao deve implicar em perda de desempenho.	
    cudaMemcpy(h_resultados,d_resultados,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost);
    cudaMemcpy(h_search_gaps,d_search_gaps,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost);
	
	
    // Guarda o que foi encontrado
    for(i=0;i<loaded;i++)
      if(h_resultados[i] != 0){
      	switch(h_resultados[i]){		
      	case SENSO:
      	  central = (char*)malloc((seqSize_an+1)*sizeof(char));
      	  if(central_cut){
      	    gap = h_search_gaps[i];
      	    strncpy(central,data[i]+gap,blocoV);
      	    central[blocoV] = '\0';
      	  }else{
      	    strncpy(central,data[i],seqSize_an+1);
      	  }

      	  if(regiao_5l){
      	    cincol = (char*)malloc((seqSize_an+1)*sizeof(char));

      	    gap = h_search_gaps[i] - dist_regiao_5l;
      	    strncpy(cincol,data[i] + gap,tam_regiao_5l);
      	    cincol[tam_regiao_5l] = '\0';
      	  }else{
      	    cincol = NULL;
      	  }
      			
      	  fsenso++;
      	  hold_event = (void*)criar_elemento_fila_event(central,cincol,SENSO);
      	  enfileirar(toStore,hold_event);
      	  break;
      	case ANTISENSO:
      	  central = (char*)malloc((seqSize_an+1)*sizeof(char));
      	  if(central_cut){
      	    gap = h_search_gaps[i];
      	    strncpy(central,data[i]+gap,blocoV);
      	    central[blocoV] = '\0';
      	  }else{
      	    strncpy(central,data[i],seqSize_an+1);
      	  }

      			
      	  if(regiao_5l){
      	    cincol = (char*)malloc((seqSize_an+1)*sizeof(char));
      	    gap = h_search_gaps[i] + dist_regiao_5l-1;
      	    strncpy(cincol,data[i] + gap,tam_regiao_5l);
      	    cincol[tam_regiao_5l] = '\0';
      	  }else{
      	    cincol = NULL;
      	  }

      	  fasenso++;
      	  hold_event = (void*)criar_elemento_fila_event(get_antisenso(central),get_antisenso(cincol),ANTISENSO);
      	  enfileirar(toStore,hold_event);
      			
      	  break;
      	}
      }
	
    checkCudaError();
	
    //////////////////////////////////////////
    // Carrega o buffer //////////////////////
    //////////////////////////////////////////
    *buffer_load = load_buffer_CUDA(data,seqSize_an);
    //cudaHostGetDevicePointer(&d_vertexes,h_vertexes,0);
    cudaMemcpyAsync(d_vertexes,h_vertexes,loaded*seqSize_an*sizeof(short int),cudaMemcpyHostToDevice,stream);
	 checkCudaError();
  }
  gettimeofday(&end_time, NULL);
	
  sec = ((end_time.tv_sec  - start_time.tv_sec) * 1000000u + end_time.tv_usec - start_time.tv_usec) / 1.e6;
  if(!silent)
    printf("Kernel de busca executado em %.2f s\n",sec);
  
  cudaFree(d_resultados);
  free(h_resultados);
  THREAD_DONE[THREAD_SEARCH] = TRUE;
  return;
}


void cudaIteracoes(const int bloco1, const int bloco2, const int seqSize_an,const int seqSize_bu,Socket *gui_socket)
{
	
	
  Buffer buffer;
  int blocoV = seqSize_bu - bloco1 - bloco2;
  int i;
  int buffer_load;
  Fila *toStore;
  double sec;
  struct timeval start_time, end_time;
	
  prepare_buffer_cuda();
  //Inicializa buffer

  buffer_load = 0;
  toStore = criar_fila("toStore");
  cudaMalloc((void**)&data,buffer_size*sizeof(char*));	
	
  THREAD_DONE[THREAD_SEARCH] = FALSE;
  THREAD_DONE[THREAD_QUEUE] = FALSE;
  THREAD_DONE[THREAD_DATABASE] = FALSE;
  
		
  gettimeofday(&start_time, NULL);
#pragma omp parallel num_threads(OMP_NTHREADS) shared(buffer) shared(buffer_load) shared(toStore)
  {		
#pragma omp sections
    {
#pragma omp section
      {
  	search_manager(&buffer_load,toStore,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV);
      }
#pragma omp section
      {
	queue_manager(toStore,&sent_to_db,&THREAD_DONE[THREAD_SEARCH]);
        THREAD_DONE[THREAD_QUEUE] = TRUE;
      }
#pragma omp section
      {
  	report_manager(gui_socket,toStore,&processadas,&sent_to_db,gui_run,verbose,silent,&fsenso,&fasenso,&THREAD_DONE[THREAD_QUEUE]);
  	THREAD_DONE[THREAD_DATABASE] = TRUE;
      }
    }
  }
  gettimeofday(&end_time, NULL);
  if(!silent){
    sec = ((end_time.tv_sec  - start_time.tv_sec) * 1000000u + end_time.tv_usec - start_time.tv_usec) / 1.e6;
    printf("Busca executada em %.2f s\n",sec);
  }
	
	
  //printf("Iterações executadas: %d.\n",iter);
  //free(tmp);
  cudaDeviceReset();
  cudaFree(d_vertexes);
  cudaFree(d_candidates);
  //cudaStreamDestroy(stream1);
  //cudaStreamDestroy(stream2);
  /*for(i=0;i<buffer_size;i++){
    cudaFreeHost(founded[i]);
    cudaFreeHost(data[i]);
    }	
    cudaFreeHost(founded);
    cudaFreeHost(data);
    cudaFreeHost(d_data);*/
  cudaFree(data);
  return;
}

void auxCUDA(char *c,const int bloco1, const int bloco2,const int seqSize_bu,Params set){
  float tempo;
  int seqSize_an;//Tamanho das sequencias analisadas
  Socket *gui_socket;

  verbose = set.verbose;
  silent = set.silent;
  debug = set.debug;
  central_cut = set.cut_central;
  gui_run = set.gui_run;
  dist_regiao_5l = set.dist_regiao_5l;
  tam_regiao_5l = set.tam_regiao_5l;
	
  if(dist_regiao_5l && tam_regiao_5l)
    regiao_5l = TRUE;
  else
    regiao_5l = FALSE;
  gui_socket = (Socket*)malloc(sizeof(Socket));
  if(!silent || gui_run)
    printf("CUDA Mode.\n");
  printString("CUDA Mode.\n",NULL);
	
  printf("Buffer size: %d\n",buffer_size);
  printStringInt("Buffer size: ",buffer_size);
	
  seqSize_an = get_setup();
	
  //Inicializa
  setup_for_cuda(c);	

  printString("Dados inicializados.\n",NULL);
  printSet(seqSize_an);
  printString("Iniciando iterações:\n",NULL);
	
  cudaIteracoes(bloco1,bloco2,seqSize_an,seqSize_bu,gui_socket);
  cudaThreadExit();

  if(gui_run){
    destroy_socket(gui_socket);
  }else{
    //db_select("SELECT * FROM events");
  }
  destroy_db_manager();
  
  return;
}
