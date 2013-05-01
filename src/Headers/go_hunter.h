#ifndef GO_HUNTER_H
#define GO_HUNTER_H
void send_setup_to_gui(struct socket *gui_socket);
void report_manager(struct socket*,struct fila*,int*,int*,int,int,int,int*,int*,int*);
void queue_manager(struct fila *toStore,int *sent_to_db,int *THREAD_SEARCH_DONE);
void aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos,Params set);
#endif
