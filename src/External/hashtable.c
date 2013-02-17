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
int dataSent;
FILE *output_file;

void criar_ghash_table(char *tempo){
	char *output_file_name;
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
  output_file_name = (char*)malloc(100*sizeof(char));
     strcpy(output_file_name,"");
    // Remove espaco da string tempo 	
	i = 0;
	j = 0;
	while(i<strlen(tempo))
		if(tempo[i] != ' ' && tempo[i] != ':' && tempo[i] != '\n'){
			output_file_name[j] = tempo[i];
			i++;
			j++;
		}else i++;
	
	 strcat(output_file_name,"_tmp");
	 strcat(output_file_name,".txt");
    
     output_file = fopen(output_file_name,"w+");               
     if(output_file == NULL){
		 printf("Impossivel abrir arquivo temporario.\n");
		 printString("Impossivel abrir arquivo temporario.",NULL);
		 exit(1);
	 }    
	 
	 free(output_file_name);                        
	return;
}

void destroy_ghash_table(){
	// Envia msg para fechar
	destroy_socket(socket);
	printf("Seqs sent: %d\n",dataSent);
	return;
}

void adicionar_ht(char *central,char *cincol,char *tipo){
	char *msg;
	char *msg_returned;
	int msg_size;
	
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
	fprintf(output_file,"%s\n",msg);
	free(msg);
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
	fclose(output_file);
	send_msg_to_socket(socket,SKT_MSG_PROCESS);
	msg_returned = get_msg_to_socket(socket);	
	return;
}
