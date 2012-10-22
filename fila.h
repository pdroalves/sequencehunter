/* This file was automatically generated.  Do not edit! */

#ifndef FILA_H
#define FILA_H

Fila* criar_fila();
void start_fila_lock();
FilaItem* criar_elemento_fila(char *seq);
void print_fila(Fila *f);
void enfileirar(Fila *f,char *novo);
void enfileirar_fila(Fila *A,Fila *B);
char* desenfileirar(Fila *f);
void despejar_fila(Fila *f,FILE *file);
int tamanho_da_fila(Fila *f);
gboolean fila_vazia(Fila *f);
void destroy(Fila *f);
#endif
