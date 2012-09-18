/* This file was automatically generated.  Do not edit! */
#if !defined(ESTRUTURAS_H)
#define ESTRUTURAS_H
#endif
#if !defined(ESTRUTURAS_H)
typedef struct lista_ligada lista_ligada;
struct lista_ligada {
	char *senso;
	int qsenso;
	int qasenso;
	struct lista_ligada* prox;
};
#endif


#ifndef PROCESSING_DATA_H
#define PROCESSING_DATA_H
void gerar_relatorio();
lista_ligada *processar(int n,pilha *p_sensos,pilha *p_antisensos);
int get_sequencias_validas(FILE**,int);
int check_seq_valida(char *p);
void quicksort(lista_ligada **l, int left, int right);
#endif