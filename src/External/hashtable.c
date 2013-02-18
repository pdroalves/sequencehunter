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
#include <ham/hamsterdb.h>
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include "../Headers/socket.h"

Socket *socket;
#define SKT_PORT 9332
#define DB_MANAGER_APP "database-manager.jar"
#define MAX_HT_FILA_SIZE 100000

int dataSent;
char *db_filename;
ham_db_t *db;

void criar_ghash_table(char *tempo){
   int i;
   int j;
	const int key_max_size = get_setup();
   
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
	 
	 db = ham_create_db(db_filename,key_max_size);
	return;
}

void destroy_ghash_table(){
	// Envia msg para fechar
    ham_destroy(db);
	printf("Seqs sent: %d\n",dataSent);
	return;
}

void adicionar_ht(char *central,char *cincol,char *tipo){
	ham_add(db,central,cincol,tipo);
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
