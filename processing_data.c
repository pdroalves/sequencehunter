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

lista_ligada* processar(GHashTable* hash_table,int n){
	
//	lista_ligada *l;
//	lista_ligada **resultados;
	//Despareados *desp;
	FILE *sensos;
	FILE *antisensos;
	int s_tipos = 0;
	int as_tipos = 0;
	int retorno;
	int i;
	char *hold;
	//GTimer *timer;
	
	//timer = g_timer_new();
	//g_timer_start(timer);
	
	//g_timer_stop(timer);
	//printf("Pilha de antisensos esvaziada em %f ms.\n",g_timer_elapsed(timer,NULL));
	
	//print_all(hash_table);
	
	printf("Tipos de senso encontrados: %d.\n",s_tipos);
	printf("Tipos de antisenso encontrados: %d.\n",as_tipos);
	
	
	printf("Procurando sensos despareados...\n");
	
	//g_timer_reset(timer);
	//g_timer_start(timer);
	//desp = recupera_despareados_ht(hash_table);
	//print_despareadas_seqs(desp->sensos,desp->antisensos);
	//printf("Sensos despareados: %d.\n",desp->sensos);
	//printf("Antisensos despareados: %d.\n",desp->antisensos);
	//printf("Processando.\n");
	
	//g_timer_stop(timer);
	//printf("Sequências despareadas encontradas em %f ms.\n",g_timer_elapsed(timer,NULL));
	
	//g_timer_reset(timer);
	//g_timer_start(timer);
	//qnt_relativa_ht(hash_table);
	//g_timer_stop(timer);
	//printf("Calculo da distribuição feita em %f ms.\n",g_timer_elapsed(timer,NULL));
	
	//Salva resultado:
	//g_timer_reset(timer);
	//g_timer_start(timer);
	write_ht_to_file(hash_table);
	//g_timer_stop(timer);
	//printf("Resultados salvos em %f ms.\n",g_timer_elapsed(timer,NULL));
	
	//LinkedList
	//g_timer_reset(timer);
	//g_timer_start(timer);	
	//l = converter_para_lista_ligada(hash_table);
	//g_timer_stop(timer);
	//printf("Conversão para Linked List em %f ms.\n",g_timer_elapsed(timer,NULL));
	
	//g_timer_reset(timer);
	//g_timer_start(timer);
	//resultados = ordena_pares(l);
	//printf("Frequencias estimadas:\n");
	//g_timer_stop(timer);
	//printf("Sequências ordenadas em %f ms.\n",g_timer_elapsed(timer,NULL));
	//imprimir_sensos(resultados);
	
	
	//g_timer_destroy(timer);
	
	i = 0;
/*	while(resultados[i]->pares != -1){
		free(resultados[i]);
		i++;
	}
	free(resultados[i]);
	free(resultados);	*/
	//free(timer);
	return NULL;
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
