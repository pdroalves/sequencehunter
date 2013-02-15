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

void criar_ghash_table(){
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
	
	msg = (char*)malloc((msg_size+1)*sizeof(char));
	
	// Avisa sobre o envio de sequencia
	//send_msg_to_socket(SKT_MSG_SND_SEQ);
	
	// Monta a msg
	strcpy(msg,"1");
	strcat(msg,central);
	strcat(msg,"2");
	if(cincol != NULL){
		strcat(msg,cincol);
		strcat(msg,"3");		
	}
	strcat(msg,tipo);
	strcat(msg," ");
	
	// Envia a sequencia
	send_msg_to_socket(socket,msg);
	msg_returned = get_msg_to_socket(socket);		
		
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
	// Envia um sinnal para o Database Manager processar os dados
	char *msg_returned;
	send_msg_to_socket(socket,SKT_MSG_PROCESS);
	msg_returned = get_msg_to_socket(socket);	
	return;
}
