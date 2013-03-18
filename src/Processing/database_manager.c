//      database_manager.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação de banco de dados SQLite3 para armazenar e contar repetições
//		de sequências. 
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

#define MAX_PER_TMP_FILE 5000000

int data_added;
char *db_filename;

void criar_db_manager(char* output_dir,char *tempo,const int key_max_size){
   int i;
   int j;
   
   db_filename = (char*)malloc(500*sizeof(char));
   if(output_dir != NULL)
	sprintf(db_filename,"%s/SH_%s.db",output_dir,tempo);
   else
	sprintf(db_filename,"SH_%s.db",tempo);
	
    // Remove espaco da string tempo 	
	i = 0;
	j = 0;
	while(i<=strlen(db_filename)){
		if(		db_filename[i] != ' ' && 
				db_filename[i] != ':' && 
				db_filename[i] != '\n'){
			db_filename[j] = db_filename[i];
			j++;
		}
		i++;
	}
	 
	 // Inicializa db, cria fila para tmp_file e inicia primeiro arquivo temporario
	 db_create(db_filename);
	db_start_transaction();
	data_added = 0;
	 //tmp_file_fila = criar_fila("Temp Files");
	 //new_tmp_file();
	return;
}

void destroy_db_manager(){
	// Envia msg para fechar
	if(data_added > 0)
		db_commit_transaction();
    db_destroy();
	printf("Seqs sent: %d\n",data_added);
	return;
}

void adicionar_db(char *central,char *cincol,int tipo){
	
	if(data_added > MAX_PER_TMP_FILE){
		db_commit_transaction();
		db_start_transaction();
		data_added = 0;
	}
	db_add(central,cincol,tipo);
	data_added++;
	
	return;
}

int tamanho_db(){
	// To-do
	return 0;		
}

char* get_database_filename(){
	return db_filename;
}
