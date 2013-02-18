#ifndef FILA_H
#define FILA_H
Fila* criar_fila(char *nome);
FilaItem* criar_elemento_fila(char *seq);
void print_fila(Fila *f);
void enfileirar(Fila *f,char *seq_central,char *seq_cincoL,int tipo);
FilaItem* desenfileirar(Fila *f);
char* desenfileirar_main(Fila *f);
int tamanho_da_fila(Fila *f);
gboolean fila_vazia(Fila *f);
void destroy(Fila *f);
#endif
