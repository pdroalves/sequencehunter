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
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include "../Headers/socket.h"

Socket *socket;
#define SKT_PORT 9332
#define DB_MANAGER_APP "database-manager.jar"
#define MAX_HT_FILA_SIZE 100000

int dataSent;
char *output_file_name_model;
char *output_file_name;
char *output_file_abs_path;
FILE *output_file;
char **tmp_fila;
int tmp_fila_size;
int tmp_files;



void open_tmp_file();
void criar_ghash_table(char *tempo){
	int i,j;
	 // Inicia database-manager
	  if(fork()==0){
		system(DB_MANAGER_APP);
		exit(0);
	  }
	  sleep(1);
     socket = criar_socket(SKT_PORT);
     if(socket == NULL){
	   printf("Não foi possível conectar ao database-manager. Tentando de novo...\n");
	   sleep(5);
		socket = criar_socket(SKT_PORT);
		if(socket == NULL){
			printString("Encerrando...",NULL);
			printf("Encerrando...\n");
			exit(1);
		}
	 }
     dataSent = 0; 
     
     // Carrega arquivo temporario
    /* tempo[strlen(tempo)-1] = '\0';
     
  */   
  output_file_name_model = (char*)malloc(100*sizeof(char));
  strcpy(output_file_name_model,"");
    // Remove espaco da string tempo 	
	i = 0;
	j = 0;
	while(i<=strlen(tempo)){
		if(tempo[i] != ' ' && 
				tempo[i] != ':' && 
				tempo[i] != '\n'){
			output_file_name_model[j] = tempo[i];
			j++;
		}
		i++;
	}
	 strcat(output_file_name_model,"_tmp");
	                
	 tmp_files = 0;   
	 open_tmp_file();   
	 tmp_fila = (char**)malloc(MAX_HT_FILA_SIZE*sizeof(char*));
	 tmp_fila_size = 0;
	return;
}
void open_tmp_file(){	
	char str[15];
	 tmp_files++;
	sprintf(str, "%d", tmp_files);
	output_file_name = (char*)malloc(100*sizeof(char));
	output_file_abs_path = (char*)malloc(400*sizeof(char));
	strcpy(output_file_name,output_file_name_model);
	strcat(output_file_name,str);
	
	output_file = fopen(output_file_name,"w+");               
     if(output_file == NULL){
		 printf("Impossivel abrir arquivo temporario.\n");
		 printString("Impossivel abrir arquivo temporario.",NULL);
		 exit(1);
		 
	 }   
	 
	 // Apenas serve para Linux
	 realpath(output_file_name,output_file_abs_path); 
}


void destroy_ghash_table(){
	// Envia msg para fechar
	 free(output_file_name_model);   
	destroy_socket(socket);
	printf("Seqs sent: %d\n",dataSent);
	return;
}

void adicionar_ht(char *central,char *cincol,char *tipo){
	char *msg;
	char *msg_returned;
	int msg_size;
	int i;
	if(tmp_fila_size >= MAX_HT_FILA_SIZE){
		for(i=0;i<MAX_HT_FILA_SIZE;i++){
			msg = tmp_fila[i];
			fprintf(output_file,"%s\n",msg);
			free(msg);	
		}
		tmp_fila_size = 0;
		fclose(output_file);
		msg = (char*)malloc(1000*sizeof(char));
		strcpy(msg,"loadFile ");
		
		strcat(msg,output_file_abs_path);
		send_msg_to_socket(socket,msg);
		msg_returned = get_msg_to_socket(socket);
		open_tmp_file();
	}
	
	
	msg_size = strlen(central) + 1 + strlen(tipo);
	if(cincol != NULL)
		msg_size=+	(strlen(cincol) + 1);
	
	msg = (char*)malloc((msg_size+5)*sizeof(char));
	
	// Monta a msg
	strcpy(msg,"1");
	strcat(msg,central);
	strcat(msg,"2");
	if(cincol != NULL){
		strcat(msg,cincol);
		strcat(msg,"3");		
	}
	strcat(msg,tipo);
	
	// Envia a sequencia
	//send_msg_to_socket(socket,msg);
	//msg_returned = get_msg_to_socket(socket);		
	//fprintf(output_file,"%s\n",msg);
	//enfileirar(tmp_fila,msg,"vazio",0);
	tmp_fila[tmp_fila_size] = msg;
	tmp_fila_size++;
	dataSent++;
	
	return;
}

int tamanho_ht(){
	char *msg_returned;
	int size;
	send_msg_to_socket(socket,SKT_MSG_GETSIZE);
	// Recebe valor
	msg_returned = get_msg_to_socket(socket);	
	size = atoi(msg_returned);
	printf("\nTamanho_ht - Received: %d\n",size);
	return atoi(msg_returned);		
}

void process_signal_ht(){
	// Envia um sinal para o Database Manager processar os dados
	char *msg_returned;
	char *msg;
	
	fclose(output_file);
	
	msg = (char*)malloc(100*sizeof(char));
	strcpy(msg,"loadFile ");
	strcat(msg,output_file_name);
	
	send_msg_to_socket(socket,msg);
	msg_returned = get_msg_to_socket(socket);	
	return;
}
