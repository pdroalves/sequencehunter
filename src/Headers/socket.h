#ifndef SOCKET_H
#define SOCKET_H
typedef struct socket Socket;
void send_msg_to_socket(Socket *sock,char *msg);
char* get_msg_to_socket(Socket *sock);
Socket* criar_socket(int port);
void destroy_socket(Socket *sock);
#endif