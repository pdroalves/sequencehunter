#ifndef GHASHTABLE_H
#define GHASHTABLE_H
#include <glib.h>
#include "estruturas.h"
GHashTable* criar_ghash_table();
value* criar_value(int pares,int qsenso,int qasenso,float qnt_relativa);
void destroy_ghash_table(GHashTable *hash_table);
value* atualizar_parametro(value *novo, value *velho);
void print_all(GHashTable *hash_table);
Pares* recupera_tipos_ht(GHashTable *hash_table);
Pares* recupera_despareados_ht(GHashTable *hash_table);
void qnt_relativa_ht(GHashTable *hash_table);
void write_ht_to_file(GHashTable *hash_table);
gboolean adicionar_ht(GHashTable *hash_table,gchar *central,gchar *cincol,value* novo_parametro);
int tamanho_ht(GHashTable *hash_table);
void write_ht_to_binary(GHashTable *hash_table,gboolean regiao5l,char *tempo);
GHashTable* read_binary_to_ht(FILE *f);
lista_ligada* converter_para_lista_ligada(GHashTable *hash_table);
#endif
