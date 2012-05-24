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
void gerar_relatorio();
float qnt_relativa();
lista_ligada *processar(pilha *p_sensos,pilha *p_antisensos);
