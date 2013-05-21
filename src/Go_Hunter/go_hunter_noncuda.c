#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#ifdef _WIN32
#include <Windows.h>
#define SLEEP(a) Sleep(1000*a)
#else
#define SLEEP(a) sleep(a)
#endif
#include "../Headers/database.h"
#include "../Headers/database_manager.h"
#include "../Headers/estruturas.h"
#include "../Headers/go_hunter_noncuda.h"
#include "../Headers/load_data.h"
#include "../Headers/operacoes.h"
#include "../Headers/nc_busca.h"
#include "../Headers/log.h"
#include "../Headers/fila.h"
#include "../Headers/nc_busca.h"
#include "../Headers/go_hunter.h"
#include "../Headers/socket.h"
#include "sqlite3.h"
#include <time.h>

// Lista de threads a serem criados
enum threads { 
  THREAD_SEARCH,
  THREAD_QUEUE,
  THREAD_DATABASE,
  OMP_NTHREADS
};
#define buffer_size 512 // Capacidade máxima do buffer
#define LOADER_QUEUE_MAX_SIZE 1e6
#define GUI_SOCKET_PORT 9332
#define GIGA 1073741824 

gboolean THREAD_DONE[OMP_NTHREADS];
omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
gboolean debug;
gboolean central_cut;
gboolean regiao_5l;
gboolean gui_run;
int dist_regiao_5l;
int tam_regiao_5l;
omp_lock_t MC_copy_lock;
int sent_to_db;
Buffer buf;
int *vertexes;
int processadas;
int fsensos,fasensos;
unsigned long nc_bytes_read=0;

const int buffer_size_NC = buffer_size;

void load_buffer_NONCuda(){
  int i,j;
  int n;
  int loaded;
  if(buf.load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
    fill_buffer(buf.seq,buf.capacidade,&loaded);//Enche o buffer e guarda a quantidade de sequências carregadas.
    n = strlen(buf.seq[0]);
    for(i=0;i<loaded;i++){	  
      convert_to_graph(buf.seq[i],n,&vertexes[i*n]);
      if( buf.seq[i][0]*(2+buf.seq[i][1]) != vertexes[i*n]){
	printf("Erro na %d - %d %s\n",loaded,vertexes[i*n],buf.seq[i]);
	exit(1);
      }
    }
    buf.load = loaded;
  } 
  return;
}

void nc_search_manager(int bloco1,int bloco2,int blocos,const int seqSize_an,Fila *toStore){
  //////////////////////////////////////////
  // Realiza as iteracoes///////////////////
  //////////////////////////////////////////
  
  int *search_gaps;
  int *resultados;
  gboolean retorno;
  int gap;
  int tam;
  int i;
  char *central;
  char *cincol;
  char *seqToSave;
  float iteration_time;
  const int blocoV = blocos-bloco1-bloco2;
  int *candidates;
  Event *hold_event;
  double sec;
  struct timeval start_time, end_time;
  processadas = 0;
  fsensos=fasensos=0;
  
  candidates = (int*)malloc(buffer_size_NC*seqSize_an*sizeof(int));
  resultados = (int*)malloc(buffer_size_NC*sizeof(int));
  search_gaps = (int*)malloc(buffer_size_NC*sizeof(int));
  
  load_buffer_NONCuda();
  
  gettimeofday(&start_time, NULL);
  while(buf.load != GATHERING_DONE){
    //Realiza loop enquanto existirem sequências para encher o buffer

    busca(bloco1,bloco2,blocos,&buf,vertexes,candidates,resultados,search_gaps);//Kernel de busca	

	    
    tam = buf.load;
    processadas+= tam;
	    
    for(i = 0; i < tam;i++){
      //Copia sequências senso e antisenso encontradas
      switch(resultados[i]){
      case SENSO:
	if(central_cut){
	  central = (char*)malloc((blocoV+1)*sizeof(char));	
	  gap = search_gaps[i];
	  strncpy(central,buf.seq[i]+gap,blocoV);
	  central[blocoV] = '\0';
	}else{				
	  central = (char*)malloc((seqSize_an+1)*sizeof(char));					
	  strncpy(central,buf.seq[i],seqSize_an+1);
	}


	if(regiao_5l){
	  cincol = (char*)malloc((tam_regiao_5l+1)*sizeof(char));

	  gap = search_gaps[i] - dist_regiao_5l;
	  strncpy(cincol,buf.seq[i] + gap,tam_regiao_5l);
	  cincol[tam_regiao_5l] = '\0';
	}else{
	  cincol = NULL;
	}
			      
	fsensos++;
	hold_event = criar_elemento_fila_event(central,cincol,SENSO);
	enfileirar(toStore,hold_event);
	break;
      case ANTISENSO:
	if(central_cut){
	  central = (char*)malloc((blocoV+1)*sizeof(char));
	  gap = search_gaps[i];
	  strncpy(central,buf.seq[i]+gap,blocoV);
	  central[blocoV] = '\0';
	}else{						
	  central = (char*)malloc((seqSize_an+1)*sizeof(char));			
	  strncpy(central,buf.seq[i],seqSize_an+1);
	}

			      
	if(regiao_5l){
	  cincol = (char*)malloc((tam_regiao_5l+1)*sizeof(char));
				      
	  gap = search_gaps[i] + dist_regiao_5l-1;
	  strncpy(cincol,buf.seq[i] + gap,tam_regiao_5l);
	  cincol[tam_regiao_5l] = '\0';
	}else{
	  cincol = NULL;
	}

	fasensos++;
	hold_event = (void*)criar_elemento_fila_event(get_antisenso(central),get_antisenso(cincol),ANTISENSO);
	enfileirar(toStore,hold_event);
	if(central != NULL)
	  free(central);
	if(cincol != NULL)
	  free(cincol);
	break;
      default:
	break;
      }
    }
	    
    // Aguarda o buffer estar cheio novamente
    buf.load = 0;
    load_buffer_NONCuda();
					
  }     
  gettimeofday(&end_time, NULL);
	
  sec = ((end_time.tv_sec  - start_time.tv_sec) * 1000000u + end_time.tv_usec - start_time.tv_usec) / 1.e6;
  if(!silent)
    printf("Kernel de busca executado em %.2f s\n",sec);
  free(resultados);
  free(search_gaps);
  THREAD_DONE[THREAD_SEARCH] = TRUE;
  return;
  //////////////////////////////////////////
  //////////////////////////////////////////
  //////////////////////////////////////////
		
}

void NONcudaIteracoes(int bloco1,int bloco2,int blocos,const int seqSize_an,Socket *gui_socket){
	
  Fila *toStore;
  int blocoV;
  double sec;
  struct timeval start_time, end_time;
  //Inicializa
  blocoV = blocos - bloco1 - bloco2+1;
  prepare_buffer(&buf,buffer_size_NC);	
  vertexes = (int*)malloc(buffer_size_NC*seqSize_an*sizeof(int));
  toStore = criar_fila("toStore");
			      
  THREAD_DONE[THREAD_SEARCH] = FALSE;
  THREAD_DONE[THREAD_QUEUE] = FALSE;
  THREAD_DONE[THREAD_DATABASE] = FALSE;
  gettimeofday(&start_time, NULL);
#pragma omp parallel num_threads(OMP_NTHREADS)
  {	
		
#pragma omp sections
    {
#pragma omp section
      {
	// Faz o processamento e adiciona resultado na queue
	nc_search_manager(bloco1,bloco2,blocos,seqSize_an,toStore);
      }
#pragma omp section
      {
	// Carrega resultados da queue e salva no db
	//nc_queue_manager(toStore);
	queue_manager(toStore,&sent_to_db,&THREAD_DONE[THREAD_SEARCH]);
	THREAD_DONE[THREAD_QUEUE] = TRUE;
      }
#pragma omp section
      {
	// Escrita de informacoes relevantes na stdout
	report_manager(gui_socket,toStore,&processadas,&sent_to_db,gui_run,verbose,silent,&fsensos,&fasensos,&THREAD_DONE[THREAD_QUEUE]);
	THREAD_DONE[THREAD_DATABASE] = TRUE;
      }
    }
  }
  gettimeofday(&end_time, NULL);
  if(!silent){
    sec = ((end_time.tv_sec  - start_time.tv_sec) * 1000000u + end_time.tv_usec - start_time.tv_usec) / 1.e6;
    printf("Busca executada em %.2f s\n",sec);
  }
  close_buffer(&buf);
  return;
}

void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,Params set){
	
  int seqSize_an;//Elementos por sequência
  float tempo;
  Socket *gui_socket;

  // Inicializa variaveis
  tempo = 0;
  verbose = set.verbose;
  silent = set.silent;
  debug = set.debug;
  central_cut = set.cut_central;
  gui_run = set.gui_run;
  dist_regiao_5l = set.dist_regiao_5l;
  tam_regiao_5l = set.tam_regiao_5l;
  if(dist_regiao_5l || tam_regiao_5l)
    regiao_5l = TRUE;
  else
    regiao_5l = FALSE;

  if(!silent)
    gui_socket = (Socket*)malloc(sizeof(Socket));
  printf("OpenMP Mode.\n");
  printString("OpenMP Mode.\n",NULL);
  printf("Buffer size: %d\n",buffer_size);
  printStringInt("Buffer size: ",buffer_size);
	
  seqSize_an = get_setup();
	
  setup_without_cuda(c);
  printString("Dados inicializados.\n",NULL);
  printSet(seqSize_an);
  printString("Iniciando iterações:\n",NULL);
	
  NONcudaIteracoes(bloco1,bloco2,blocos,seqSize_an,gui_socket);
    
  printString("Iterações terminadas. Tempo: ",NULL);
  print_tempo(tempo);
	

  // Destruir a DB aqui eh gambiarra, mas tem de ser feito sempre antes de encerrar o socket
  if(gui_run){
    destroy_socket(gui_socket);
  }else{
    //db_select("SELECT * FROM events");
  }
  destroy_db_manager();
  return;	
}

