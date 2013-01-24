/* This file was automatically generated.  Do not edit! */
#if !defined(ESTRUTURAS_H)
#define ESTRUTURAS_H

struct lista_ligada{
	char *senso;
	int pares;
	int qsenso;
	int qasenso;
	float qnt_relativa;
	struct lista_ligada* prox;
};
#endif

int busca_lista_as(lista_ligada *l,char *seq);
int busca_lista_s(lista_ligada *l,char *seq);
int conta_posicoes(char *seq);
void adicionar_elemento(lista_ligada *lista, lista_ligada *novo);
lista_ligada* criar_lista();
void remover_elemento(lista_ligada *atual, lista_ligada *anterior);
Despareados* recupera_despareados(lista_ligada *l);
void imprimir_lista_ligada(lista_ligada *resultados,gboolean silent,gboolean gui_run);
int qnt_relativa(lista_ligada* l);
lista_ligada** ordena_pares(lista_ligada* l);
void g_hash_table_adapter_to_linked_list(gchar *seq,value *entry,lista_ligada *l);
