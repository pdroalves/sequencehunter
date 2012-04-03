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
int busca_lista_as(lista_ligada *l,char *seq);
int busca_lista_s(lista_ligada *l,char *seq);
int conta_posicoes(char *seq);
void adicionar_elemento(lista_ligada *final,char *seq);
lista_ligada* criar_lista();
