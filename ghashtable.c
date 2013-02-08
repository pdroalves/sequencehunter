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
#include "estruturas.h"
#include "log.h"

#define SKT_PORT 9332
#define SKT_MSG_HELLO "hi"
#define SKT_MSG_CLOSE "bye"
#define SKT_MSG_SND_SEQ "incomingseq" 

GSocketConnection * connection = NULL;
GSocketClient * client;
GOutputStream * ostream = NULL;

void send_msg_to_socket(char *msg){
  GError * error = NULL;
	g_output_stream_write  (ostream,
							  msg,
							  strlen(msg),
							  NULL,
							  &error);
}

void criar_ghash_table(){
  GError * error = NULL;
  
  g_type_init();
  
	client =  g_socket_client_new();
	connection = g_socket_client_connect_to_host (client,
                                               (gchar*)"localhost",
                                                SKT_PORT, /* your port goes here */
                                                NULL,
                                                &error);
    if(error != NULL){
		printString("ERRO!\n\tNão foi possivel estabelecer conexão com a base de dados:",error->message);
		printString("Encerrando...",NULL);
		exit(1);
	}
    
    ostream = g_io_stream_get_output_stream (G_IO_STREAM (connection));  
    
    send_msg_to_socket(SKT_MSG_HELLO);
    // Falta implementar uma conexao para checar se esta tudo ok
                                                  
	return;
}

void destroy_ghash_table(){
	// Envia msg para fechar
  GError * error = NULL;
	send_msg_to_socket(SKT_MSG_CLOSE);
	g_socket_client_connect_to_host_finish(client,NULL,&error);
	return;
}

void adicionar_ht(gchar *central,gchar *cincol,char *tipo){
	char *msg;
	int msg_size;
	
	msg_size = strlen(central) + 1 + strlen(tipo);
	if(cincol != NULL)
		msg_size=+	(strlen(cincol) + 1);
	
	msg = (char*)malloc((msg_size+1)*sizeof(char));
	
	// Avisa sobre o envio de sequencia
	send_msg_to_socket(SKT_MSG_SND_SEQ);
	
	// Monta a msg
	strcpy(msg,central);
	strcat(msg," ");
	if(cincol != NULL){
		strcat(msg,cincol);
		strcat(msg," ");		
	}
	strcat(msg,tipo);
	
	// Envia a sequencia
	send_msg_to_socket(msg);		
		
	free(msg);
	
	return;
}
