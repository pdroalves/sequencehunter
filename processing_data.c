#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "ghashtable.h"
#include "estruturas.h"
#include "load_data.h"
#include "log.h"
#define TAM_MAX 10000

int check_seq_valida(char *p);



int check_seq_valida(char *p){
	int i;
	int n;
	
	n = strlen(p);
	
	if(n == 0) return 0;
	
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

lista_ligada* processar(GHashTable* hash_table,int n,int max_events){
	
	Despareados *desp;
	lista_ligada *l;
	lista_ligada *resultados;
	int s_tipos = 0;
	int as_tipos = 0;
	
	//print_all(hash_table);
	
	// To-do: Funcao para contar tipos de senso e antisenso
	printf("Tipos de senso encontrados: %d.\n",s_tipos);
	printf("Tipos de antisenso encontrados: %d.\n",as_tipos);
	
	printf("Procurando sensos despareados...\n");
	
	desp = recupera_despareados_ht(hash_table);
	print_despareadas_seqs(desp->sensos,desp->antisensos);
	printf("Sensos despareados: %d.\n",desp->sensos);
	printf("Antisensos despareados: %d.\n",desp->antisensos);
	
	// Calcula quantidade relativa dos elementos
	qnt_relativa_ht(hash_table);

	//LinkedList
	l = converter_para_lista_ligada(hash_table);
	
	// Ordena os max_events maiores elementos
	resultados = ordena_pares(l,max_events);
	
	return resultados;
}

void imprimir(lista_ligada *resultados,int max_events){
	
	printf("Frequencias estimadas:\n");
	imprimir_lista_ligada(resultados);
	
	return;
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
