#ifndef FILA_H
#define FILA_H
Fila* criar_fila(char *nome);
Event* criar_elemento_fila_event(char *seq_full,char *seq_central,char *seq_cincoL,int tipo);
void print_fila(Fila *f);
void enfileirar(Fila *f,void *data);
Event* desenfileirar(Fila *f);
int tamanho_da_fila(Fila *f);
gboolean fila_vazia(Fila *f);
void destroy(Fila *f);
#endif
