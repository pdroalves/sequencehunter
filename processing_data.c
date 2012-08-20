#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "estruturas.h"
#include "log.h"
#include "linkedlist.h"
#include "pilha.h"
#define TAM_MAX 10000


int check_seq_valida(char *p);

int get_sequencias_validas(FILE **f,int files){
	int j = 0;
	int notify_tamanho_variavel = 0;
	int seqs_validas = 0;
	int m = 0;
	int n = 0;
	char *tmp;
	
	tmp = (char*)malloc(TAM_MAX*sizeof(char));
	for(j=0;j < files;j++){		
		while(feof(f[j]) == 0){
				fscanf(f[j],"%s",tmp);		
				n = strlen(tmp);
				if(check_seq_valida(tmp)){	
					seqs_validas++;
					//printf("%d\n",seqs_validas);
					if(m == 0) m = n;
					else if(n!=m && notify_tamanho_variavel == 0) notify_tamanho_variavel == 1;
				}
				tmp[0] = '\0';
		}
	}
	for(j=0;j<files;j++) rewind(f[j]);
	return seqs_validas;
}

	int check_seq_valida(char *p){
	int i;
	int n = strlen(p);
	
	for(i = 0; i < n;i++){
			switch(p[i]){
				case 'A':
				break;
				case 'a':
				break;
				case 'C':
				break;
				case 'c':
				break;
				case 'G':
				break;
				case 'g':
				break;
				case 'T':
				break;
				case 't':
				break;
				default:
					return 0;
			}
	}
	
	return 1;
}

lista_ligada* processar(pilha *p_sensos,pilha *p_antisensos){
	
	lista_ligada *l;
	lista_ligada **resultados;
	Despareados *desp;
	int s_tipos = 0;
	int as_tipos = 0;
	int retorno;
	l = criar_lista();
	char *hold;
	
	print_total_seqs(tamanho_da_pilha(p_sensos),tamanho_da_pilha(p_antisensos));
	
	//Processa sensos
	while(pilha_vazia(p_sensos) == 1){
		hold = desempilha(p_sensos);
		retorno = busca_lista_s(l,hold);
		if(retorno == 1)
			s_tipos++;
		free(hold);
	}
	
	while( pilha_vazia(p_antisensos)== 1){
		hold = desempilha(p_antisensos);
		retorno = busca_lista_as(l,hold);
		if(retorno == 1)
			as_tipos++;
		free(hold);
	}
	/*
	printf("Tipos de senso encontrados: %d.\n",s_tipos);
	printf("Tipos de antisenso encontrados: %d.\n",as_tipos);
	printf("Procurando sensos despareados...\n");
	desp = recupera_despareados(l);
	print_despareadas_seqs(desp->sensos,desp->antisensos);
	printf("Sensos despareados: %d.\n",desp->sensos);
	printf("Antisensos despareados: %d.\n",desp->antisensos);
	
	printf("Processando.\n");
	qnt_relativa(l);
	resultados = ordena_pares(l);
	printf("Frequencias estimadas:\n");
	imprimir_sensos(resultados);
	
	
	int i = 0;
	while(resultados[i]->pares != -1){
		free(resultados[i]);
		i++;
	}
	free(resultados[i]);
	free(resultados);	
	*/
	
	
	destroy(p_sensos);
	destroy(p_antisensos);
	return l;
}

void swap(lista_ligada** a, lista_ligada** b) {
  lista_ligada *tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
return;
}
 
int particione(lista_ligada **l, int left, int right) {
  int i;
  int j;
 
  i = left;
  for (j = left + 1; j <= right; ++j) {
    if (&l[j]->qnt_relativa < &l[left]->qnt_relativa){
      i++;
      swap(&l[i], &l[j]);
    }
  }
  swap(&l[left], &l[i]);
 
  return i;
}
 
void quicksort(lista_ligada **l, int left, int right) {
  int r;
 
  if (right > left) {
    r = particione(l, left, right);
    quicksort(l, left, r - 1);
    quicksort(l, r + 1, right);
  }
  
}

void gerar_relatorio(){
		//		Deve criar arquivo data-hora.txt com a sequência buscada, a quantidade de sequências lidas na biblioteca, tamanho do buffer,
		//	tempo de processamento, tipos e quantidade de sensos e antisensos encontrados (inclusive sua quantidade relativa) e quantidade de
		//	sensos e antisensos encontrados mas que foram descartados por não possuirem pares.
	
	
}