#ifndef GHASHTABLE_H
#define GHASHTABLE_H

GHashTable* criar_ghash_table();
value* criar_value(int pares,int qsenso,int qasenso,float qnt_relativa);
void adicionar_elemento(GHashTable *hash_table,gchar *seq,value *novo_parametro);
value* atualizar_parametro(value *novo, value *velho);
void print_all(GHashTable *hash_table);
Despareados* recupera_despareados_ht(GHashTable *hash_table);
void qnt_relativa_ht(GHashTable *hash_table);
void write_ht_to_file(GHashTable *hash_table);

#endif
