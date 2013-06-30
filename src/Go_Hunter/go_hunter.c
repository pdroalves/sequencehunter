//      aux.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "../Headers/database_manager.h"
#include "../Headers/estruturas.h"
#include "../Headers/go_hunter_cuda.h"
#include "../Headers/go_hunter_noncuda.h"
#include "../Headers/fila.h"
#include "../Headers/socket.h"
#include "../Headers/log.h"
#include "sqlite3.h"
#ifdef _WIN32
#include <Windows.h>
#define SLEEP(a) Sleep(1000*a)
#define gettime(a) GetTickCount(a)
#else
#define SLEEP(a) sleep(a)
#define gettime(a) gettimeofday(a)
#endif
#define GUI_SOCKET_PORT 9332
#define GIGA 1073741824 

int tmp = 0;
int sent_to_db = 0;

void send_setup_to_gui(Socket *gui_socket){
  char *msg;
  
  if(gui_socket == NULL){
	  printf("Socket não configurado. Encerrando...\n");
	  exit(1);
  }
  
  msg = (char*)malloc(MAX_SOCKET_MSG_SIZE*sizeof(char));
  
  sprintf(msg,"DB %s",get_database_filename());
  
  send_msg_to_socket(gui_socket,msg);
  
  sprintf(msg,"Log %s",get_log_filename());
  
  send_msg_to_socket(gui_socket,msg);
  
  free(msg);
  return;
}

void report_manager(	Socket *gui_socket,
						Fila* toStore,
						int *p,
						gboolean gui_run,
						gboolean verbose,
						gboolean silent,
						int *fsensos,
						int *fasensos,
			long double *readCount,
						int *THREAD_DONE_QUEUE
						){
  clock_t cStartClock;
  int queue_size;
  int pos_queue_size;
  int pre_sent_to_db;
  int pos_sent_to_db;
  int count;
  int diff;
  // FILE* fp_enchimento;
  // FILE* fp_esvaziamento;
  char *msg;
  float sqlite3_mem_used;
  int port = GUI_SOCKET_PORT;
  int new_p;
  int old_p;
  float rate_processing;
  float rate_enqueue;
  int sleep_time = 1;
  
    // if(verbose && !silent){
      // fp_enchimento = fopen("enchimento.dat","w");
      // fp_esvaziamento = fopen("esvaziamento.dat","w");
    // }
  count = 0;
  
  
  if(gui_run){
	  send_setup_to_gui(gui_socket);
  }
  
  while(!(*THREAD_DONE_QUEUE)){
    
    queue_size = tamanho_da_fila(toStore);
    pre_sent_to_db = sent_to_db;
    old_p = *p;
    SLEEP(sleep_time);
    pos_queue_size = tamanho_da_fila(toStore);
    pos_sent_to_db = sent_to_db;
    new_p = *p;
    
    count++;
    
    diff = pos_sent_to_db - pre_sent_to_db;
    rate_processing = (new_p-old_p)/((float)(sleep_time));
    rate_enqueue = (pos_sent_to_db - pre_sent_to_db)/((float)(sleep_time));
    
    if(gui_run){		
      msg = (char*)malloc(MAX_SOCKET_MSG_SIZE*sizeof(char));
      sprintf(msg,"T%dS%dAS%dSPS%.0fBR%lf",*p,*fsensos,*fasensos,rate_enqueue,*readCount);
      send_msg_to_socket(gui_socket,msg);
      free(msg);
    }
    if(verbose && !silent){
	  sqlite3_mem_used = sqlite3_memory_used();
      printf("DB memory used: %.2f GB\n",sqlite3_mem_used/(float)GIGA);
      printf("Processed sequences: %d - S: %d, AS: %d\n",*p,*fsensos,*fasensos);
      printf("DB queue: %d\n",pos_queue_size);
      printf("Processing rate: %.1f seq/s\n",rate_processing);
      printf("Enqueue rate: %.1f seqs/s\n\n",rate_enqueue);
      //fprintf(fp_enchimento,"%d %d\n",count,pos_queue_size-queue_size);
     //fprintf(fp_esvaziamento,"%d %d\n",count,pos_sent_to_db - pre_sent_to_db);
    }	
  }
    //if(verbose && !silent){
	  //fclose(fp_enchimento);
	  //fclose(fp_esvaziamento);
	//}
  return;
}


void queue_manager(Fila *toStore,int *THREAD_SEARCH_DONE){
	Event *hold;
	char *central;
	char *cincoL;
	float tempo;
		
	while(!(*THREAD_SEARCH_DONE) || tamanho_da_fila(toStore) > 0){
	  if(tamanho_da_fila(toStore) > 0){
	 	 hold = desenfileirar(toStore);
		  if(hold != NULL){
			central = hold->seq_central;
			cincoL = hold->seq_cincoL;

			if(hold == NULL){
			  printf("Memory error - Queue.\n");
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
	  }else{
	  	SLEEP(1);
	  }
	}	
  
  return;
}

void aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos,Params set,Socket *socket){
	if(CUDA)
		auxCUDA(c,bloco1,bloco2,blocos,set,socket);
	else
		auxNONcuda(c,bloco1,bloco2,blocos,set,socket);
    return; 
}

