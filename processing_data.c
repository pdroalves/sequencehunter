#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "linkedlist.h"
#include "ghashtable.h"
#include "estruturas.h"
#include "load_data.h"
#include "log.h"

#define TAM_MAX 10000

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

lista_ligada* processar(int n,int max_events,gboolean silent,gboolean gui_run){
	
	Pares *desp;
	Pares *tipos;
	lista_ligada *l;
	lista_ligada *resultados;
/*
	//print_all(hash_table);
	if(hash_table == NULL){
	  if(!silent)
		printf("Nao foram encontradas sequências com pareamento.\n");
		resultados = criar_lista();
	}else{
		if(tamanho_ht(hash_table) == 0){
	  if(!silent)
			printf("Nao foram encontradas sequências com pareamento.\n");
			resultados = criar_lista();
		}else{
			tipos = recupera_tipos_ht(hash_table);
			desp = recupera_despareados_ht(hash_table);
		  
			if(!silent){
				 printf("Tipos de senso encontrados: %d.\n",tipos->sensos);
				printf("Tipos de antisenso encontrados: %d.\n",tipos->antisensos);
				printf("Procurando sensos despareados...\n");
				free(tipos);
			}
			print_despareadas_seqs(desp->sensos,desp->antisensos);
			if(!silent){
				printf("Sensos despareados: %d.\n",desp->sensos);
				printf("Antisensos despareados: %d.\n",desp->antisensos);
			}
		
			// Calcula quantidade relativa dos elementos
			qnt_relativa_ht(hash_table);

			//LinkedList
			l = converter_para_lista_ligada(hash_table);
			
			// Ordena os max_events maiores elementos
			resultados = ordena_pares(l,max_events);
		}
	}
	free(desp);
	* */
	return NULL;
}

void imprimir(lista_ligada *resultados,char *tempo,int max_events,gboolean silent,gboolean gui_run){
	if(!silent)
	printf("Frequencias estimadas:\n");
	//imprimir_lista_ligada(resultados,tempo,silent,gui_run);
	
	return;
}
  

