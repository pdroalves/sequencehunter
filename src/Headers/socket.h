#ifndef SOCKET_H
#define SOCKET_H
void send_msg_to_socket(struct socket *sock,char *msg);
char* get_msg_to_socket(struct socket *sock);
void criar_socket(struct socket *sock,int port);
void destroy_socket(struct socket *sock);
void configure_socket(struct socket*);
#endif
