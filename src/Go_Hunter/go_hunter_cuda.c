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

#define buffer_size 4096 // Capacidade máxima do buffer
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
enum threads { 
  THREAD_BUFFER_LOADER,
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


void buffer_manager(	int *buffer_load,
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
  h_candidates = (short int*)malloc(buffer_size*seq_size*sizeof(short int));
  cudaMalloc((void**)&d_vertexes,buffer_size*seq_size*sizeof(short int));
  cudaMalloc((void**)&d_candidates,buffer_size*seq_size*sizeof(short int));
    
  //////////////////////////////////////////
  // Carrega o buffer //////////////////////
  //////////////////////////////////////////
  while(*buffer_load != GATHERING_DONE){//Looping até o final do buffer
    if(*buffer_load == 0){      
      loaded = load_buffer_CUDA(data,seq_size);
      cudaMemcpy(d_vertexes,h_vertexes,loaded*seq_size*sizeof(short int),cudaMemcpyHostToDevice);
      *buffer_load = loaded;
    }
  }
		
  THREAD_DONE[THREAD_BUFFER_LOADER] = TRUE;
  //////////////////////////////////////////
  //////////////////////////////////////////
  //////////////////////////////////////////	
}

void search_manager(int *buffer_load,
		    Fila *toStore,
		    const int seqSize_an,
		    const int seqSize_bu,
		    int bloco1,
		    int bloco2,
		    int blocoV,
		    cudaStream_t stream1,
		    cudaStream_t stream2)
{	
  int i;
  short int *h_resultados;
  short int *d_resultados;
  short int *h_search_gaps;
  short int *d_search_gaps;
  char **h_founded;
  char **d_founded;
  char **d_tmp_founded;
  char *hold_seq;
  char *central;
  char *cincol;
  int loaded;
  int hold;
  int p;
  int last_p;
  float iteration_time;
  int gap;
  int wave_size;
  int wave_processed_diff;
  cudaEvent_t startK,stopK;
  cudaEvent_t start,stop;
  cudaEvent_t startV,stopV;
  char **local_data;
  float elapsedTimeK,elapsedTime,elapsedTimeV;
  Event *hold_event;
  gboolean retorno;

  fsenso=fasenso=0;

  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventCreate(&startK);
  cudaEventCreate(&stopK);
  cudaEventCreate(&startV);
  cudaEventCreate(&stopV);

  // Alloc: resultados de cada iteracao
  h_resultados = (short int*)malloc(buffer_size*sizeof(short int));
  h_search_gaps = (short int*)malloc(buffer_size*sizeof(short int));
  cudaMalloc((void**)&d_resultados,buffer_size*sizeof(short int));
  cudaMalloc((void**)&d_search_gaps,buffer_size*sizeof(short int));

  // Alloc: Seqs encontradas
  //CPU
  cudaHostAlloc((void**)&h_founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
  for(i=0;i<buffer_size;i++)
    cudaHostAlloc((void**)&h_founded[i],(seqSize_an+1)*sizeof(char),cudaHostAllocDefault);

  //GPU	
  d_tmp_founded = (char**)malloc(buffer_size*sizeof(char*));
  for(i=0;i<buffer_size;i++)
    cudaHostAlloc((void**)&d_tmp_founded[i],(seqSize_an+1)*sizeof(char),cudaHostAllocDefault);

  cudaMalloc((void**)&d_founded,buffer_size*sizeof(char**));
  cudaMemcpy(d_founded,d_tmp_founded,buffer_size*sizeof(char*),cudaMemcpyHostToDevice);

  // Alloc: Vetor temporario
  local_data = (char**)malloc(buffer_size*sizeof(char*));
  for(i=0;i<buffer_size;i++)
    local_data[i] = (char*)malloc((seqSize_an+1)*sizeof(char));
  hold_seq = (char*)malloc(seqSize_an*sizeof(char));

  iteration_time = 0;

  processadas = 0;

  while( *buffer_load == 0){
  }//Aguarda para que o buffer seja enchido pela primeira vez

  cudaEventRecord(start,0);
  while( *buffer_load != GATHERING_DONE){
    //Realiza loop enquanto existirem sequencias para encher o buffer
    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
	
    cudaEventElapsedTime(&elapsedTime,start,stop);
    if(debug&&!silent)
      printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
    iteration_time += elapsedTime;
	
    loaded = *buffer_load;
    // Execuca iteracao
    cudaEventRecord(startK,0);
    k_busca(*buffer_load,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,d_vertexes,d_candidates,d_resultados,d_search_gaps,d_founded,stream2);//Kernel de busca
    cudaEventRecord(stopK,0);	
    cudaEventSynchronize(stopK);
    cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
    if(debug&&!silent)
      printf("Execucao da busca em %.2f ms\n",elapsedTimeK);
	
    // Guarda tempo gasto na iteracao
    iteration_time += elapsedTimeK;
	
	
    cudaEventRecord(start,0);
	
    // Inicia processamento dos resultados
    processadas += loaded;
    cudaStreamSynchronize(stream2);
	
    // Em casos reais, cada iteracao possuirah poucos eventos. Portanto, a copia de dados para 
    // o host serah bastante casual e esse trecho nao deve implicar em perda de desempenho.	
    cudaMemcpy(h_resultados,d_resultados,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost);
    cudaMemcpy(h_search_gaps,d_search_gaps,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost);
    //for(i=0;i<buffer_size;i++)
    //	if(h_resultados[i] != 0)
    //		cudaMemcpyAsync(h_founded[i],data[i],seqSize_an*sizeof(char),cudaMemcpyDeviceToHost,stream2);
	
    //cudaStreamSynchronize(stream2);
	
	
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
      	  wave_size++;
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
      	  wave_size++;
      	  hold_event = (void*)criar_elemento_fila_event(get_antisenso(central),get_antisenso(cincol),ANTISENSO);
      	  enfileirar(toStore,hold_event);
      			
      	  break;
      	}
      }
		
    // Libera para o thread buffer_manager carregar mais sequencias
    *buffer_load = 0;		
	
    checkCudaError();
	
	
    // Aguarda o buffer estar cheio novamente
    cudaEventRecord(startV,0);
    while(*buffer_load == 0 && !THREAD_DONE[THREAD_BUFFER_LOADER] || tamanho_da_fila(toStore) > LOADER_QUEUE_MAX_SIZE ){}
    cudaEventRecord(stopV,0);	
    cudaEventSynchronize(stopV);
    cudaEventElapsedTime(&elapsedTimeV,startV,stopV);
	
    // Evita desincronização
    if(	*buffer_load == 0 && 
	THREAD_DONE[THREAD_BUFFER_LOADER])
      {
	*buffer_load = GATHERING_DONE;
      }
    
    if(debug && !silent)
      printf("Tempo aguardando encher o buffer: %.2f ms\n",elapsedTimeV);
	
  }
  if(!silent)
    if(iteration_time > 10000)
      printf("Busca realizada em %.2f min.\n",iteration_time/(float)60000);
    else 
      printf("Busca realizada em %.2f ms.\n",iteration_time);
			
  for(i=0;i<buffer_size;i++)
    free(local_data[i]);
  free(local_data);
  cudaFree(d_resultados);
  for(i=0;i<buffer_size;i++)
    cudaFreeHost(h_founded[i]);
  cudaFreeHost(h_founded);
  free(h_resultados);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);
  cudaEventDestroy(startK);
  cudaEventDestroy(stopK);
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
  cudaStream_t stream;
	
  prepare_buffer_cuda();
  //Inicializa buffer
  cudaStreamCreate(&stream);

  buffer_load = 0;
  toStore = criar_fila("toStore");
  cudaMalloc((void**)&data,buffer_size*sizeof(char*));	
	
  THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
  THREAD_DONE[THREAD_SEARCH] = FALSE;
  THREAD_DONE[THREAD_QUEUE] = FALSE;
  THREAD_DONE[THREAD_DATABASE] = FALSE;
  
		
#pragma omp parallel num_threads(OMP_NTHREADS) shared(buffer) shared(buffer_load) shared(stream) shared(toStore)
  {		
#pragma omp sections
    {
#pragma omp section
      {
	buffer_manager(&buffer_load,seqSize_an,stream);
      }
#pragma omp section
      {
	search_manager(&buffer_load,toStore,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,stream,stream);
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
  
	
  //printf("Iterações executadas: %d.\n",iter);
  //free(tmp);
  cudaDeviceReset();
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
    db_select("SELECT * FROM events");
  }
  destroy_db_manager();
  
  return;
}
