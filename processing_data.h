#ifndef PROCESSING_DATA_H
#define PROCESSING_DATA_H
#include <glib.h>
#include "estruturas.h"
void gerar_relatorio();
lista_ligada* processar(GHashTable* hash_table,int n,int max_events,gboolean silent,gboolean gui_run);
void imprimir(lista_ligada *resultados,char *tempo,int max_events,gboolean silent,gboolean gui_run);
int check_seq_valida(char *p);
void quicksort(lista_ligada **l, int left, int right);
int check_seq_valida(char *p);
#endif
