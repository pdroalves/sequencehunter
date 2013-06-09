#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gio/gio.h>
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include <gnu/libc-version.h>
#ifdef _WIN32
#include <Windows.h>
#define SLEEP(a) Sleep(1000*a)
#else
#define SLEEP(a) sleep(a)
#endif

#define SKT_PORT 9332
char* get_msg_to_socket(Socket *sock){
  GError * error = NULL;
  char *msg;

  msg = (char*)malloc(MAX_SEQ_SIZE*sizeof(char));
  g_input_stream_read(  sock->istream,
      msg,
      MAX_SEQ_SIZE*sizeof(char),
      NULL,
      &error);
  return msg;
}

void send_msg_to_socket(Socket *sock,char *msg){
  char *result;
  GError * error = NULL;
  if(sock == NULL || sock->connection == NULL){    
          printf("Unable to establish connection to GUI.\nAborting...");
          printString("Unable to establish connection to GUI.\nAborting...",NULL);
          exit(1);
  }
  g_output_stream_write  (sock->ostream,
			  msg,
			  strlen(msg)+1,
			  NULL,
			  &error);
  get_msg_to_socket(sock);
  
  }

void criar_socket(Socket *sock,int port){
  GError *error = NULL;
  char *result;
  GSocketAddress *address;

  if(gnu_get_libc_version() < "2.36")
  // A versao da GLib para Windows ainda precisa disso
    g_type_init ();
  sock->client =  g_socket_client_new();
  g_socket_client_set_timeout(sock->client,3000);
  sock->connection = g_socket_client_connect_to_host (sock->client,
						      (gchar*)"localhost",
						      9332,
						      NULL,
						      &error);
   if(error != NULL){
	sock = NULL;
	return;
    }

    address = g_socket_client_get_local_address(sock->client);

    
    sock->istream = g_io_stream_get_input_stream (G_IO_STREAM (sock->connection));  
    sock->ostream = g_io_stream_get_output_stream (G_IO_STREAM (sock->connection));  
    
    send_msg_to_socket(sock,SKT_MSG_HELLO);  
    return;
}

void destroy_socket(Socket *sock){
  GError * error = NULL;
  char *msg_returned;
  send_msg_to_socket(sock,SKT_MSG_CLOSE);
}


void configure_socket(Socket *gui_socket){
  char *msg;
      msg = (char*)malloc(MAX_SOCKET_MSG_SIZE*sizeof(char));
    
    
    criar_socket(gui_socket,SKT_PORT);
    if(gui_socket == NULL && gui_socket->connection == NULL){
        SLEEP(5);
        criar_socket(gui_socket,SKT_PORT);
        if(gui_socket == NULL && gui_socket->connection == NULL){
          printf("Unable to establish connection to GUI.\nAborting...");
          printString("Unable to establish connection to GUI.\nAborting...",NULL);
          exit(1);
        }
    }

    return;
}