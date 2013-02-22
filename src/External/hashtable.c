//      ghashtable.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação de GHashTable para armazenar e contar repetições
//		de sequências. Cria nova GHashtable, adiciona elemento, 
//
//		21/08/2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gio/gio.h>
#include "../Headers/database.h"
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include "../Headers/socket.h"
#include <cuda_runtime_api.h> 

Socket *socket;
#define SKT_PORT 9332
#define DB_MANAGER_APP "database-manager.jar"
#define MAX_HT_FILA_SIZE 100000

int dataSent;
char *db_filename;
FILE *output;

struct ham_db_t *db; /* hamsterdb database object */
struct ham_env_t* env;

void criar_ghash_table(char *tempo,const int key_max_size){
   int i;
   int j;
   
   db_filename = (char*)malloc(100*sizeof(char));
   strcpy(db_filename,"SHunter - DB - ");
   
    // Remove espaco da string tempo 	
	i = 0;
	j = 0;
	while(i<=strlen(tempo)){
		if(tempo[i] != ' ' && 
				tempo[i] != ':' && 
				tempo[i] != '\n'){
			db_filename[j] = tempo[i];
			j++;
		}
		i++;
	}
	 strcat(db_filename,".db");
	 
	 db_init_lock();
	 db_create(db_filename,key_max_size);
	//output = fopen(db_filename,"w+");
	return;
}

void open_and_load_file(char *filename){
	FILE *fp;
	char db_filename[] = "database.db"; 
	char *seq = (char*)malloc(200*sizeof(char));
	char *tipo = (char*)malloc(10*sizeof(char));
	int count;
	float elapsedTime;
	cudaEvent_t start,stop;
	
	
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	fp = fopen(filename,"r");
	db_create(db_filename,NULL);
	
	
	count = 0;
	while(!feof(fp)){
		fscanf(fp,"%s - %s",seq,tipo);
		cudaEventRecord(start,0);
		
		db_add(seq,NULL,tipo);
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		cudaEventElapsedTime(&elapsedTime,start,stop);
			
		
		count++;
		printf("%d - %f seq/ms\n",count,1/elapsedTime);
	}
	
}


void destroy_ghash_table(){
	// Envia msg para fechar
    db_destroy();
    //fclose(output);
	printf("Seqs sent: %d\n",dataSent);
	return;
}

void adicionar_ht(char *central,char *cincol,char *tipo){
	db_add(central,cincol,tipo);
	//fprintf(output,"%s - %s\n",central,tipo);
	dataSent++;
	
	return;
}

int tamanho_ht(){
	// To-do
	return 0;		
}

void process_signal_ht(){
	// To-do
	return;
}
