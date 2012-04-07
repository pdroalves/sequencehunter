#include <stdio.h>
#include "estruturas.h"
#include "log.h"
#include "linkedlist.h"
#include "pilha.h"

lista_ligada* processar(pilha *p_sensos,pilha *p_antisensos){
	
	lista_ligada *l;
	int s_tipos = 0;
	int retorno;
	l = criar_lista();
	
	//Processa sensos
	while(pilha_vazia(p_sensos) == 1){
		retorno = busca_lista_s(l,desempilha(p_sensos));
		if(retorno == 1)
			s_tipos++;
	}
	
	
	while( pilha_vazia(p_antisensos)== 1){
		retorno = busca_lista_as(l,desempilha(p_antisensos));
		if(retorno == 1){
			printf("Não encontrei!\n");
		}
	}
	
	printf("Tipos de senso encontrados: %d.\n",s_tipos);
	printf("Procurando sensos sem antisensos.\n");
	printf("Sensos solitários: %d.\n",limpando_sensos(l));
	
	imprimir_sensos(l);
	
	return l;
}
