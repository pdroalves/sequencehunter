//      ghashtable.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação de GHashTable para armazenar e contar repetições
//		de sequências. Cria nova GHashtable, adiciona elemento, 
//
//		21/08/2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <omp.h>
#include "estruturas.h"
#include "log.h"
#include "linkedlist.h"
/*#include <omp.h>
#include "estruturas.h"
#include "log.h"
#include "operacoes.h"
#include "processing_data.h"
#define MIN(a,b) a>=b?b:a
*/


value* atualizar_parametro(value *novo, value *velho);


void free_key(gchar *seq){
	free(seq);
	return;
}

void free_value(value *valor){
	free(valor);
	return;
}

GHashTable* criar_ghash_table(){
	//return g_hash_table_new(g_str_hash,g_str_equal,free,free);
	return g_hash_table_new_full(g_str_hash,g_str_equal,free_key,free_value);
}

void destroir_ghash_table(GHashTable *hash_table){
	g_hash_table_destroy (hash_table);
	free(hash_table); 
	return;
}

value* criar_value(int pares,int qsenso,int qasenso,float qnt_relativa){
	value *valor;
	valor = (value*)malloc(sizeof(value));
	valor->pares = pares;
	valor->qsenso = qsenso;
	valor->qasenso = qasenso;
	valor->qnt_relativa = qnt_relativa;
	return valor;	
}

gboolean adicionar_ht(GHashTable *hash_table,gchar *seq,value* novo_parametro){
	//Retorna TRUE se a key ainda não existir na hast_table. FALSE caso contrário.
	value* velho_parametro;
	gboolean answer;
	
	#pragma omp critical
	{
		velho_parametro = g_hash_table_lookup(hash_table,seq);
		if(velho_parametro == NULL){
			g_hash_table_insert(hash_table,seq,novo_parametro);
			answer = TRUE;
		}else{
			g_hash_table_insert(hash_table,seq,atualizar_parametro(novo_parametro,velho_parametro));
			answer = FALSE;
		}
	}
	return answer;
}
void print_all(GHashTable *hash_table){
	GHashTableIter iter;
	gchar *key;
	value *entry;
	
	g_hash_table_iter_init(&iter,hash_table);
	while(g_hash_table_iter_next(&iter,&key,&entry)){
		printf("%s\n",key);
		printf("%d - %d\n",entry->qsenso,entry->qasenso);
		print_despareadas(key,entry->qsenso,entry->qasenso);
	}
	return;
}

value* atualizar_parametro(value *novo, value *velho){
	value* atualizacao;
	atualizacao = novo;
	atualizacao->pares+=velho->pares;
	atualizacao->qsenso+=velho->qsenso;
	atualizacao->qasenso+=velho->qasenso;
	atualizacao->qnt_relativa+=velho->qnt_relativa;
	return atualizacao;
}

void recupera_despareados_func(gchar *seq,value *entry,Despareados *desp){
	int diff;
	diff = entry->qsenso - entry->qasenso;
	//printf("%s - S:%d - As:%d\n",atual->senso,atual->qsenso,atual->qasenso);
	//print_despareadas(seq,entry->qsenso,entry->qasenso);
	if(diff != 0){
		 if(diff > 0) desp->sensos+=diff;
		 else desp->antisensos+=-diff;
	}
	return;
}

Despareados* recupera_despareados_ht(GHashTable *hash_table){
	Despareados *desp;
	
	desp = (Despareados*)malloc(sizeof(Despareados));
	
	g_hash_table_foreach(hash_table,recupera_despareados_func,desp);
	
	return desp;
}


void qnt_relativa_total_count_func(gchar *seq,value *entry,long int *total){
	entry->pares = MIN(entry->qsenso,entry->qasenso);
	*total += entry->pares;
	return;
}

void qnt_relativa_update_func(gchar *seq,value *entry,long int *total){
	entry->qnt_relativa = entry->pares / (float) *total;
	return;
}

void qnt_relativa_ht(GHashTable *hash_table){
	long int total = 0;
	//Contabiliza a quantidade total de pares encontrados
	g_hash_table_foreach(hash_table,qnt_relativa_total_count_func,&total);
	
	//Grava quantidade relativa de cada tipo de pares encontrados
	g_hash_table_foreach(hash_table,qnt_relativa_update_func,&total);
	return;
}

lista_ligada* converter_para_lista_ligada(GHashTable *hash_table){
	lista_ligada *l;
	
	l = criar_lista();
	g_hash_table_foreach(hash_table,g_hash_table_adapter_to_linked_list,l);
	
	return l;
}

void ht_to_binary(gchar *seq,value *entry,FILE *f){
	int size = strlen(seq)+1;
	fwrite(&size,sizeof(int),1,f);
	fwrite(seq,sizeof(char),size,f);
	fwrite(&entry->qsenso,sizeof(int),1,f);
	fwrite(&entry->qasenso,sizeof(int),1,f);
}

void write_ht_to_binary(GHashTable *hash_table){
	FILE *f;
	int size = tamanho_ht(hash_table);
	char outname[50];
	char *tempo;	
	time_t t;
	
	time(&t);
	tempo = ctime(&t);
	tempo[strlen(tempo)-1] = '\0';
	
	strcpy(outname,"output - ");
	strcat(outname,tempo);
	strcat(outname,".shunt");
	
	f = fopen(outname,"w");
	fwrite(&size,sizeof(int),1,f);
	g_hash_table_foreach(hash_table,ht_to_binary,f);
	fclose(f);
	printf("Filtragem bruta salva em %s\n",outname);
	printString("Filtragem bruta salva em",outname);
	return;
}

GHashTable* read_binary_to_ht(FILE *f){
	GHashTable *hash_table;
	int i;
	int size;
	int seq_len;
	int qsenso;
	int qasenso;
	float qnt_relativa;
	char *seq;
	
	hash_table = criar_ghash_table();
	
	fread(&size,sizeof(int),1,f);
	for(i=0;i<size;i++){		
		fread(&seq_len,sizeof(int),1,f);
		seq = (char*)malloc(seq_len*sizeof(char));
		fread(seq,sizeof(char),seq_len,f);
		fread(&qsenso,sizeof(int),1,f);
		fread(&qasenso,sizeof(int),1,f);
		adicionar_ht(hash_table,seq,criar_value(0,qsenso,qasenso,0));
	}
	
	return hash_table;
}

int tamanho_ht(GHashTable *hash_table){
	return g_hash_table_size(hash_table);
}
