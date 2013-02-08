#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "estruturas.h"
int busca_lista_as(lista_ligada *l,char *seq);
int busca_lista_s(lista_ligada *l,char *seq);
int conta_posicoes(char *seq);
void adicionar_elemento(lista_ligada *lista, lista_ligada *novo);
lista_ligada* criar_lista();
void remover_elemento(lista_ligada *atual, lista_ligada *anterior);
Pares* recupera_despareados(lista_ligada *l);
void imprimir_lista_ligada(lista_ligada *resultados,char *tempo,gboolean silent,gboolean gui_run);
int qnt_relativa(lista_ligada* l);
lista_ligada* ordena_pares(lista_ligada* l,int max);
#endif
