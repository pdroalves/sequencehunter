/* This file was automatically generated.  Do not edit! */

#ifndef PILHA_H
#define PILHA_H

char* desempilha(pilha *tp);
int conta_posicoes(char *seq);
pilha* criar_elemento_pilha(char*);
void empilha(pilha*,pilha*);
pilha* criar_pilha();
int tamanho_da_pilha(pilha*);
void destroy(pilha *tp);
int pilha_vazia(pilha *tp);
void despejar(pilha* p,FILE *filename);
void despejar_seq(char *seq,FILE *f);
char* carrega_do_arquivo(int n,FILE *filename);
void empilhar_pilha(pilha*,pilha*);
#endif
