#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "ghashtable.h"
#include "estruturas.h"
#include "log.h"
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
					else if(n!=m && notify_tamanho_variavel == 0) notify_tamanho_variavel = 1;
				}
				tmp[0] = '\0';
		}
	}
	for(j=0;j<files;j++) rewind(f[j]);
	return seqs_validas;
}

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

lista_ligada* processar(int n){
	
	lista_ligada *l;
	lista_ligada **resultados;
	GHashTable* hash_table;
	Despareados *desp;
	FILE *sensos;
	FILE *antisensos;
	int s_tipos = 0;
	int as_tipos = 0;
	int retorno;
	int i;
	char *hold;
	//GTimer *timer;
	
	//timer = g_timer_new();
	hash_table = criar_ghash_table();
	//g_timer_start(timer);
	
	sensos = fopen("tmp_sensos","r");
	antisensos = fopen("tmp_antisensos","r");

	//Descarrega tudo para o HD e limpa a memória
	/*despejar(p_sensos,sensos);
	despejar(p_antisensos,antisensos);
	destroy(p_sensos);
	destroy(p_antisensos);
	*/
	//Processa sensos
	#pragma omp parallel shared(n) shared(sensos) shared(hash_table) shared(s_tipos) private(hold)
	{  
		hold = carrega_do_arquivo(n,sensos);
		while(hold != NULL && check_seq_valida(hold)){
			retorno = adicionar_ht(hash_table,hold,criar_value(0,1,0,0));
			if(retorno)
				s_tipos++;
			hold = carrega_do_arquivo(n,sensos);
		}
	}
	//g_timer_stop(timer);
	//printf("Pilha de sensos esvaziada em %f ms.\n",g_timer_elapsed(timer,NULL));
	
	//g_timer_reset(timer);
	//g_timer_start(timer);
	//Processa antisensos
	#pragma omp parallel shared(n) shared(antisensos) shared(hash_table) shared(as_tipos) private(hold)
	{
		hold = carrega_do_arquivo(n,antisensos);
		while( hold != NULL && check_seq_valida(hold)){
			retorno = adicionar_ht(hash_table,hold,criar_value(0,0,1,0));
			if(retorno)
				as_tipos++;
			hold = carrega_do_arquivo(n,antisensos);
		}
	}
	fclose(sensos);
	fclose(antisensos);
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

void gerar_relatorio(){
		//		Deve criar arquivo data-hora.txt com a sequência buscada, a quantidade de sequências lidas na biblioteca, tamanho do buffer,
		//	tempo de processamento, tipos e quantidade de sensos e antisensos encontrados (inclusive sua quantidade relativa) e quantidade de
		//	sensos e antisensos encontrados mas que foram descartados por não possuirem pares.
	
	
}
