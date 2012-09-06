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

GHashTable* criar_ghash_table(){
	//return g_hash_table_new(g_str_hash,g_str_equal,free,free);
	return g_hash_table_new(g_str_hash,g_str_equal);
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
	
	velho_parametro = g_hash_table_lookup(hash_table,seq);
	if(velho_parametro == NULL){
		g_hash_table_insert(hash_table,seq,novo_parametro);
		return TRUE;
	}else{
		g_hash_table_insert(hash_table,seq,atualizar_parametro(novo_parametro,velho_parametro));
		return FALSE;
	}
}
void print_all(GHashTable *hash_table){
	GHashTableIter iter;
	gchar *key;
	value *entry;
	
	g_hash_table_iter_init(&iter,hash_table);
	while(g_hash_table_iter_next(&iter,&key,&entry)){
		printf("%s\n",key);
		printf("%d - %d - %d - %f\n",entry->pares,entry->qsenso,entry->qasenso,entry->qnt_relativa);
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
	print_despareadas(seq,entry->qsenso,entry->qasenso);
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

void write_ht_to_file_func(char *seq,value *entry,ResultFiles *result){
	if(entry->qsenso == 1){
		fprintf(result->file1,"%s - S: %d - As: %d -> %f\n",seq,entry->qsenso,entry->qasenso,entry->qnt_relativa);
	}
	
	if(entry->qsenso >= 2 && entry->qsenso <= 99){
		fprintf(result->file299,"%s - S: %d - As: %d -> %f\n",seq,entry->qsenso,entry->qasenso,entry->qnt_relativa);
	}
	
	if(entry->qsenso >= 100 && entry->qsenso <= 999){
		fprintf(result->file100999,"%s - S: %d - As: %d -> %f\n",seq,entry->qsenso,entry->qasenso,entry->qnt_relativa);		
	}
	
	if(entry->qsenso >= 1000 && entry->qsenso <= 9999){
		fprintf(result->file10009999,"%s - S: %d - As: %d -> %f\n",seq,entry->qsenso,entry->qasenso,entry->qnt_relativa);		
	}
	
	if(entry->qsenso >= 10000){
		//fprintf(result->file10000,"%s\n",seq);		
		
		fprintf(result->file10000,"%s - S: %d - As: %d -> %f\n",seq,entry->qsenso,entry->qasenso,entry->qnt_relativa);		
	}
}

void write_ht_to_file(GHashTable *hash_table){
	
	ResultFiles result;
	result.file1 = fopen("senso_seq_1.txt","w");
	result.file299 = fopen("senso_seq_2-99.txt","w");
	result.file100999 = fopen("senso_seq_100-999.txt","w");
	result.file10009999 = fopen("senso_seq_1000-9999.txt","w");
	result.file10000 = fopen("senso_seq_10000.txt","w");
	
	g_hash_table_foreach(hash_table,write_ht_to_file_func,&result);
	
	fclose(result.file1);
	fclose(result.file299);
	fclose(result.file100999);
	fclose(result.file10009999);
	fclose(result.file10000);
	
	return;
}
