#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gio/gio.h>
#include "../Headers/estruturas.h"
#include "../Headers/log.h"

#define SKT_PORT 9332

void send_msg_to_socket(Socket *sock,char *msg){
  GError * error = NULL;
  g_output_stream_write  (sock->ostream,
			  msg,
			  strlen(msg),
			  NULL,
			  &error);
  }

char* get_msg_to_socket(Socket *sock){
  GError * error = NULL;
  char msg[MAX_SEQ_SIZE];
  strcpy(msg,"");
  g_input_stream_read(	sock->istream,
			msg,
			MAX_SEQ_SIZE*sizeof(char),
			NULL,
			&error);
  return msg;
}

void criar_socket(Socket *sock,int port){
  GError *error = NULL;
  
  g_type_init ();
  
  sock->client =  g_socket_client_new();
  sock->connection = g_socket_client_connect_to_host (sock->client,
						      (gchar*)"localhost",
						      port,
						      NULL,
						      &error);
   if(error != NULL){
	sock = NULL;
	return;
    }
    
    sock->istream = g_io_stream_get_input_stream (G_IO_STREAM (sock->connection));  
    sock->ostream = g_io_stream_get_output_stream (G_IO_STREAM (sock->connection));  
    
    send_msg_to_socket(sock,SKT_MSG_HELLO);  
   // get_msg_to_socket(sock);   
    return;
}

void destroy_socket(Socket *sock){
  GError * error = NULL;
  char *msg_returned;
  send_msg_to_socket(sock,SKT_MSG_CLOSE);
  g_socket_client_connect_to_host_finish(sock->client,NULL,&error);
}
