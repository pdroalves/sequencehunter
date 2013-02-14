#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "../Headers/linkedlist.h"
#include "../Headers/hashtable.h"
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"
#include "../Headers/log.h"

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

	//print_all(hash_table);
	if(tamanho_ht() != 0){	
			printf("Existem sequencias!\n");
			process_signal_ht();
		}else{
		if(!silent)
			printf("Nao foram encontradas sequências com pareamento.\n");
			
			resultados = criar_lista();
	}
	
	return NULL;
}

void imprimir(lista_ligada *resultados,char *tempo,int max_events,gboolean silent,gboolean gui_run){
	if(!silent)
	printf("Frequencias estimadas:\n");
	//imprimir_lista_ligada(resultados,tempo,silent,gui_run);
	
	return;
}
  

