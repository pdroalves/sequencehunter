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
#include <time.h>
#include "../Headers/database.h"
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include "../Headers/fila.h"

#define MAX_PER_TMP_FILE 10000

int data_added;
char *db_filename;
FILE *tmp_file;
Fila *tmp_file_fila;

void new_tmp_file(){
	tmp_file = fopen(tempnam(NULL,"SH_"),"w");
	tmp_file = tmpfile();
	if(!tmp_file){
		printf("Erro ao abrir arquivo temporario\n");
		exit(1);
	}
}

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
	 strcat(db_filename,".sqlite3");
	 
	 // Inicializa db, cria fila para tmp_file e inicia primeiro arquivo temporario
	 db_create(db_filename,key_max_size);
	 tmp_file_fila = criar_fila("Temp Files");
	 new_tmp_file();
	return;
}

void destroy_ghash_table(){
	// Envia msg para fechar
    db_destroy();
    //fclose(output);
	printf("Seqs sent: %d\n",data_added);
	return;
}

void adicionar_ht(char *central,char *cincol,int tipo){
	/*if(data_added > MAX_PER_TMP_FILE){
		enfileirar(tmp_file_fila,tmp_file);
		new_tmp_file();
		data_added = 0;
	}
	if(tipo == SENSO)
		fprintf(tmp_file,"\t%s\t%s",central,"S");
	else
		fprintf(tmp_file,"%s\t%s",central,"AS");
	*/
	db_add(central,NULL,SENSO);
	data_added++;
	
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

int tmp_queue_size(){
	return tamanho_da_fila(tmp_file_fila);
}

void load_from_tmp_file(){
	FILE *fp;
	char *seq;
	char *tipo;
	float elapsedTime;
	int total_loaded;
    clock_t cStartClock;
    char sInputBuf [MAX_SEQ_SIZE] = "\0";
	
	if(tmp_queue_size() > 0){
		seq = (char*)malloc(200*sizeof(char));
		tipo = (char*)malloc(10*sizeof(char));
		fp = (FILE*)desenfileirar(tmp_file_fila);
		rewind(fp);
		
		cStartClock = clock();
		
		total_loaded=0;
		db_start_transaction();
		while(!feof(fp)){
			fgets(sInputBuf,MAX_SEQ_SIZE,fp);
			seq = strtok(sInputBuf,"\t");
			tipo = strtok(NULL,"\t");
			if(strcmp(tipo,"S")==0)
				db_add(seq,NULL,SENSO);
			else
				db_add(seq,NULL,ANTISENSO);
			
			
			total_loaded++;
		}
		db_commit_transaction();
		
		printf("\t\tTo db: %f seq/s\n\n",total_loaded/((clock() - cStartClock) / (double)CLOCKS_PER_SEC));
		
		//free(seq);
		fclose(fp);
	}
	
}

void open_and_load_file(char *filename){
	FILE *fp;
	char db_filename[] = "database.sqlite3"; 
	char *seq = (char*)malloc(200*sizeof(char));
	char *tipo = (char*)malloc(10*sizeof(char));
	int count;
	float elapsedTime;
    clock_t cStartClock;
	
    cStartClock = clock();
	
	fp = fopen(filename,"r");
	db_create(db_filename,"22");
	
	count = 0;
	db_start_transaction();
	while(!feof(fp)){
		fscanf(fp,"%s - %s",seq,tipo);
		if(strcmp(tipo,"S") == 0)
			adicionar_ht(seq,NULL,SENSO);
		else
			adicionar_ht(seq,NULL,ANTISENSO);
		
		count++;
		if(count == 50000){
			db_commit_transaction();
			printf("%d - %f seq/s\n",count,50000/((clock() - cStartClock) / (double)CLOCKS_PER_SEC));
			db_start_transaction();
			count = 0;
		}
	}
	
}
