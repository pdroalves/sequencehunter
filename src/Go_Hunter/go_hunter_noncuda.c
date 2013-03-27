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
#include "../Headers/database_manager.h"
#include "../Headers/estruturas.h"
#include "../Headers/go_hunter_noncuda.h"
#include "../Headers/load_data.h"
#include "../Headers/operacoes.h"
#include "../Headers/nc_busca.h"
#include "../Headers/log.h"
#include "../Headers/fila.h"
#include "../Headers/socket.h"
#include "sqlite3.h"

// Lista de threads a serem criados
enum threads { 
  THREAD_BUFFER_LOADER,
  THREAD_SEARCH,
  THREAD_QUEUE,
  THREAD_DATABASE,
  OMP_NTHREADS
};
#define buffer_size 4096 // Capacidade máxima do buffer
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
int p;
int fsensos,fasensos;
Socket *gui_socket;
unsigned long nc_bytes_read=0;

const int buffer_size_NC = buffer_size;

void load_buffer_NONCuda(int n){
  if(buf.load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
	  fill_buffer(buf.seq,buf.capacidade,&buf.load);//Enche o buffer e guarda a quantidade de sequências carregadas.			
  } 
  return;
}

void nc_buffer_manager(int n){
  //////////////////////////////////////////
  // Carrega o buffer //////////////////////
  //////////////////////////////////////////
  while(buf.load != -1){//Looping até o final do buf
	  //printf("%d.\n",buf.load);
	  if(buf.load == 0){
		  load_buffer_NONCuda(n);
	  }
  }
  THREAD_DONE[THREAD_BUFFER_LOADER] = TRUE;
  //////////////////////////////////////////
  //////////////////////////////////////////
  //////////////////////////////////////////	
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
  cudaEvent_t startK,stopK,start,stop;
		  cudaEvent_t startV,stopV;
  float elapsedTimeK,elapsedTime,elapsedTimeV;
  float iteration_time;
  const int blocoV = blocos-bloco1-bloco2;
  int wave_size;
  int wave_processed_diff;
  Event *hold_event;
  THREAD_DONE[THREAD_SEARCH] = FALSE;
  p = 0;
  fsensos=fasensos=0;
  
  resultados = (int*)malloc(buffer_size_NC*sizeof(int));
  search_gaps = (int*)malloc(buffer_size_NC*sizeof(int));
		  
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
  cudaEventCreate(&startK);
  cudaEventCreate(&stopK);
  cudaEventCreate(&startV);
  cudaEventCreate(&stopV);
  
  iteration_time = 0;
  wave_size = 0;
  wave_processed_diff = 0;
  
  while( buf.load == 0){
  }//Aguarda para que o buffer seja enchido pela primeira vez
  
  cudaEventRecord(start,0);
  while(buf.load != GATHERING_DONE || 
		  THREAD_DONE[THREAD_BUFFER_LOADER] == FALSE){
    //Realiza loop enquanto existirem sequências para encher o buffer
    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime,start,stop);
    iteration_time += elapsedTime;
    //if(verbose == TRUE && silent != TRUE)	
    //	printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
    if(debug){
	    if(!silent)
	    printf("Tempo até retornar para busca em %f ms\n",elapsedTime);
	    printString("Retorno da busca:\n",NULL);
	    //print_tempo_optional(elapsedTime);
    }
    cudaEventRecord(start,0);

    cudaEventRecord(startK,0);
	    busca(bloco1,bloco2,blocos,&buf,resultados,search_gaps);//Kernel de busca					
    cudaEventRecord(stopK,0);
    cudaEventSynchronize(stopK);

    cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
    iteration_time += elapsedTimeK;
    if(debug){
	    if(!silent)
	    printf("Execucao da busca em %f ms\n",elapsedTimeK);
	    printString("Execucao da busca:\n",NULL);
	    //print_tempo_optional(elapsedTimeK);
    }
    cudaEventRecord(start,0);
		    
	    
    tam = buf.load;
    p += tam;
	    
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
		      wave_size++;
		      hold_event = criar_elemento_fila_event(central,cincol,SENSO);
		      enfileirar(toStore,hold_event);
		     /* adicionar_db(central,cincol,SENSO);
			sent_to_db++;
		      if(central)
			free(central);
		      if(cincol)
			free(cincol);*/
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
		      wave_size++;
		      hold_event = (void*)criar_elemento_fila_event(get_antisenso(central),get_antisenso(cincol),ANTISENSO);
		      enfileirar(toStore,hold_event);
		      if(central != NULL)
			free(central);
		      if(cincol != NULL)
			free(cincol);
		      /*adicionar_db(central,cincol,SENSO);
			sent_to_db++;
		      if(central)
			free(central);
		      if(cincol)
			free(cincol);*/
		      break;
		      default:
		      break;
	      }
    }
	    
	    // Aguarda o buffer estar cheio novamente
    cudaEventRecord(startV,0);
	if(buf.load > 0)
		buf.load = 0;
    while(	buf.load==0 && 
			!THREAD_DONE[THREAD_BUFFER_LOADER]
			|| 
			tamanho_da_fila(toStore) > LOADER_QUEUE_MAX_SIZE ){}
    cudaEventRecord(stopV,0);						
    cudaEventSynchronize(stopV);
    cudaEventElapsedTime(&elapsedTimeV,startV,stopV);

    if(debug && !silent)
	    printf("Tempo aguardando encher o buffer: %.2f ms\n",elapsedTimeV);						
  }     

  if(!silent)
      printf("Busca realizada em %.2f ms.\n",iteration_time);
  
  free(resultados);
  free(search_gaps);
  THREAD_DONE[THREAD_SEARCH] = TRUE;
  return;
  //////////////////////////////////////////
  //////////////////////////////////////////
  //////////////////////////////////////////
		
}

void nc_queue_manager(Fila *toStore){
 Event *hold;
 char *central;
 char *cincoL;
  float tempo;
  
  sent_to_db =0;
		
    while(tamanho_da_fila(toStore)> 0 || !THREAD_DONE[THREAD_SEARCH]){
      hold = (Event*)desenfileirar(toStore);
      if(hold != NULL){
	central = hold->seq_central;
	cincoL = hold->seq_cincoL;
	
	if(hold == NULL){
	  printf("Erro alocando memoria - Queue.\n");
	  exit(1);
	}
	
	adicionar_db(central,cincoL,hold->tipo);
	  
	sent_to_db++;
	if(central != NULL)
	  free(central);
	if(cincoL != NULL)
	  free(cincoL);
	free(hold);
      }
    }	
  
  THREAD_DONE[THREAD_QUEUE] = TRUE;
  return;
}

void nc_send_setup_to_gui(){
  char *msg;
  
  if(gui_socket == NULL){
	  printf("Socket não configurado. Encerrando...\n");
	  exit(1);
  }
  
  msg = (char*)malloc(MAX_SOCKET_MSG_SIZE*sizeof(char));
  
  sprintf(msg,"DB %s",get_database_filename());
  
  send_msg_to_socket(gui_socket,msg);
  get_msg_to_socket(gui_socket);
  
  sprintf(msg,"Log %s",get_log_filename());
  
  send_msg_to_socket(gui_socket,msg);
  get_msg_to_socket(gui_socket);
  
  free(msg);
  return;
}

void nc_report_manager(Fila* toStore){
  clock_t cStartClock;
  int queue_size;
  int pos_queue_size;
  int pre_sent_to_db;
  int pos_sent_to_db;
  int count;
  int diff;
  FILE* fp_enchimento;
  FILE* fp_esvaziamento;
  char *msg;
  float sqlite3_mem_used;
  int port = GUI_SOCKET_PORT;
  
    if(verbose && !silent){
      fp_enchimento = fopen("enchimento.dat","w");
      fp_esvaziamento = fopen("esvaziamento.dat","w");
    }
  count = 0;
  
  
  if(gui_run){
	  msg = (char*)malloc(MAX_SOCKET_MSG_SIZE*sizeof(char));
	  
	  gui_socket = (Socket*)malloc(sizeof(Socket));
	  
	  criar_socket(gui_socket,port);
	  if(gui_socket == NULL){
	      SLEEP(5);
	      criar_socket(gui_socket,port);
	      if(gui_socket == NULL){
		      printf("Não foi possível estabelecer conexão com a GUI.\nEncerrando...");
		      printString("Não foi possível estabelecer conexão com a GUI.\nEncerrando...",NULL);
		      exit(1);
	      }
	  }
	  nc_send_setup_to_gui();
  }
  
  while(!THREAD_DONE[THREAD_QUEUE]){
    
    queue_size = tamanho_da_fila(toStore);
    pre_sent_to_db = sent_to_db;
    SLEEP(1);
    pos_queue_size = tamanho_da_fila(toStore);
    pos_sent_to_db = sent_to_db;
    
    count++;
    
    diff = pos_sent_to_db - pre_sent_to_db;
    
    if(gui_run){
      sprintf(msg,"T%dS%dAS%dSPS%dBR%d",p,fsensos,fasensos,diff,0);
      send_msg_to_socket(gui_socket,msg);
      get_msg_to_socket(gui_socket);
    }
    
    if(verbose && !silent){
	  sqlite3_mem_used = sqlite3_memory_used();
      printf("DB memory used: %.2f GB\n",sqlite3_mem_used/(float)GIGA);
      printf("Sequencias processadas: %d - S: %d, AS: %d\n",p,fsensos,fasensos);
      printf("Enchimento: %d seq/s - %d\n",pos_queue_size-queue_size,pos_queue_size);
      printf("Esvaziamento: %d seq/s\n\n",pos_sent_to_db - pre_sent_to_db);
      fprintf(fp_enchimento,"%d %d\n",count,pos_queue_size-queue_size);
      fprintf(fp_esvaziamento,"%d %d\n",count,pos_sent_to_db - pre_sent_to_db);
    }	
  }
    if(verbose && !silent){
	  fclose(fp_enchimento);
	  fclose(fp_esvaziamento);
	}
  THREAD_DONE[THREAD_DATABASE] = TRUE;
  return;
}


void NONcudaIteracoes(int bloco1,int bloco2,int blocos,const int seqSize_an){
	
	Fila *toStore;
	int blocoV;
	//Inicializa
	blocoV = blocos - bloco1 - bloco2+1;
	prepare_buffer(&buf,buffer_size_NC);	
	toStore = criar_fila("toStore");
			      
	THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
	THREAD_DONE[THREAD_SEARCH] = FALSE;
	THREAD_DONE[THREAD_QUEUE] = FALSE;
	THREAD_DONE[THREAD_DATABASE] = FALSE;
	#pragma omp parallel num_threads(OMP_NTHREADS) shared(toStore) shared(seqSize_an) shared(buf)
	{	
		
	  #pragma omp sections
	  {
	      #pragma omp section
	      {
		      nc_buffer_manager(seqSize_an);
	      }
	      #pragma omp section
	      {
		      nc_search_manager(bloco1,bloco2,blocos,seqSize_an,toStore);
	      }
	      #pragma omp section
	      {
		      nc_queue_manager(toStore);
	      }
	      #pragma omp section
	      {
		      nc_report_manager(toStore);
	      }
	  }
	}
	close_buffer(&buf);
	return;
}

void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,Params set){
	
	int seqSize_an;//Elementos por sequência
	float tempo;

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
	printf("OpenMP Mode.\n");
	printString("OpenMP Mode.\n",NULL);
	printf("Buffer size: %d\n",buffer_size);
	printStringInt("Buffer size: ",buffer_size);
	
	seqSize_an = get_setup();
	
	setup_without_cuda(c);
	printString("Dados inicializados.\n",NULL);
	printSet(seqSize_an);
	printString("Iniciando iterações:\n",NULL);
	
	//cudaEventRecord(start,0);
	NONcudaIteracoes(bloco1,bloco2,blocos,seqSize_an);
	//cudaEventRecord(stop,0);
	//cudaEventSynchronize(stop);
	//cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	
	if(gui_run){
		// Destruir a DB aqui eh gambiarra, mas tem de ser feito sempre antes de encerrar o socket
		destroy_db_manager();
		destroy_socket(gui_socket);
	}
	return;	
}

