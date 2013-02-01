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

int INICIO_SUBHT = 1;
int FINAL_SUBHT = 0;

value* atualizar_parametro(value *novo, value *velho);


void free_key(gchar *seq){
	if(seq != NULL)
		free(seq);
	return;
}

void free_value(value *valor){
	if(valor != NULL)
		free(valor);
	return;
}

GHashTable* criar_ghash_table(){
	return g_hash_table_new_full(g_str_hash,g_str_equal,(GDestroyNotify) free_key,(GDestroyNotify) free_value);
}

void destroy_ghash_table(GHashTable *hash_table){
	g_hash_table_destroy (hash_table);
	//free(hash_table); 
	return;
}

value* criar_value(int pares,int qsenso,int qasenso,float qnt_relativa){
	value *valor;
	valor = (value*)malloc(sizeof(value));
	valor->pares = pares;
	valor->qsenso = qsenso;
	valor->qasenso = qasenso;
	valor->qnt_relativa = qnt_relativa;
	valor->regiao_5l = NULL;
	return valor;	
}

gboolean adicionar_ht(GHashTable *hash_table,gchar *central,gchar *cincol,value* novo_parametro){
	//Retorna TRUE se a key ainda não existir na hash_table. FALSE se ja existir.
	value *parametro_atualizado;
	value* velho_parametro;
	gboolean answer;

	velho_parametro = (value*) g_hash_table_lookup(hash_table,central);

	if(velho_parametro == NULL){
		if(cincol){
			novo_parametro->regiao_5l = criar_ghash_table();
			adicionar_ht(novo_parametro->regiao_5l,cincol,NULL,criar_value(novo_parametro->pares,
																			novo_parametro->qsenso,
																			novo_parametro->qasenso,
																			novo_parametro->qnt_relativa));
		}
		g_hash_table_insert(hash_table,central,novo_parametro);
		answer = TRUE;
	}else{
		parametro_atualizado = atualizar_parametro(novo_parametro,cincol,velho_parametro);
		g_hash_table_insert(hash_table,central,parametro_atualizado);
		answer = FALSE;
	}
	
	return answer;
}
void print_all(GHashTable *hash_table){
	GHashTableIter iter;
	gchar *key;
	value *entry;
	
	g_hash_table_iter_init(&iter,hash_table);
	while(g_hash_table_iter_next(&iter,(gpointer*)&key,(gpointer*)&entry)){
		printf("%s\n",key);
		printf("%d - %d\n",entry->qsenso,entry->qasenso);
		print_despareadas(key,entry->qsenso,entry->qasenso);
	}
	return;
}

value* atualizar_parametro(value *novo, gchar *cincol,value *velho){
	value* atualizacao;
	atualizacao = novo;
	atualizacao->pares+=velho->pares;
	atualizacao->qsenso+=velho->qsenso;
	atualizacao->qasenso+=velho->qasenso;
	atualizacao->qnt_relativa+=velho->qnt_relativa;

	if(cincol){
		// Atualiza hashtable com a sequencia a esquerda
		atualizacao->regiao_5l = velho->regiao_5l;
		adicionar_ht(atualizacao->regiao_5l,cincol,NULL,criar_value(novo->pares,
																		novo->qsenso,
																		novo->qasenso,
																		novo->qnt_relativa));
	}

	return atualizacao;
}

void recupera_tipos_func(gpointer *seqH,gpointer *entryH,gpointer *despH){
	Pares *desp;

	desp = (Pares*) despH;

	desp->sensos++;
	desp->antisensos++;
	return;
}

Pares* recupera_tipos_ht(GHashTable *hash_table){
	Pares *desp;
	
	desp = (Pares*)malloc(sizeof(Pares));
	desp->sensos = 0;
	desp->antisensos = 0;

	g_hash_table_foreach(hash_table,recupera_tipos_func,desp);
	
	return desp;
}

void recupera_despareados_func(gpointer *seqH,gpointer *entryH,gpointer *despH){
	int diff;
	gchar *seq;
	value *entry;
	Pares *desp;

	seq = (gchar*) seqH;
	entry = (value*) entryH;
	desp = (Pares*) despH;

	diff = entry->qsenso - entry->qasenso;
	//printf("%s - S:%d - As:%d\n",atual->senso,atual->qsenso,atual->qasenso);
	//print_despareadas(seq,entry->qsenso,entry->qasenso);
	if(diff != 0){
		 if(diff > 0) desp->sensos+=diff;
		 else desp->antisensos+=-diff;
	}
	return;
}

Pares* recupera_despareados_ht(GHashTable *hash_table){
	Pares *desp;
	
	desp = (Pares*)malloc(sizeof(Pares));
	desp->sensos = 0;
	desp->antisensos = 0;

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
	g_hash_table_foreach(hash_table,(GHFunc) g_hash_table_adapter_to_linked_list,l);
	
	return l;
}

void ht_interna_to_binary(gchar *seq,value *entry,FILE *f){
	int size = strlen(seq)+1;
	fwrite(&INICIO_SUBHT,sizeof(int),1,f);	
	fwrite(&size,sizeof(int),1,f);
	fwrite(seq,sizeof(char),size,f);
	fwrite(&entry->qsenso,sizeof(int),1,f);
	fwrite(&entry->qasenso,sizeof(int),1,f);
}

void ht_to_binary(gchar *seq,value *entry,FILE *f){
	int size = strlen(seq)+1;
	int sub_ht_size;
	fwrite(&size,sizeof(int),1,f);
	fwrite(seq,sizeof(char),size,f);
	fwrite(&entry->qsenso,sizeof(int),1,f);
	fwrite(&entry->qasenso,sizeof(int),1,f);
	if(entry->regiao_5l != NULL){
		g_hash_table_foreach(entry->regiao_5l,(GHFunc) ht_interna_to_binary,f);
		fwrite(&FINAL_SUBHT,sizeof(int),1,f);
	}
}

void write_ht_to_binary(GHashTable *hash_table,gboolean regiao5l){
	FILE *f;
	int i;
	int size = tamanho_ht(hash_table);
	char outname[50];
	char outname_tmp[50];
	char *tempo;	
	time_t t;
	
	time(&t);
	tempo = ctime(&t);
	tempo[strlen(tempo)-1] = '\0';
	
	strcpy(outname_tmp,"output - ");
	strcat(outname_tmp,tempo);
	strcat(outname_tmp,".shunt");

	for(i=0;i <= strlen(outname_tmp); i++){
		if(outname_tmp[i] == ':')
			outname[i] = ' ';
		else
			outname[i] = outname_tmp[i];
	}
	
	f = fopen(outname,"w");
	fwrite(&size,sizeof(int),1,f);
	fwrite(&regiao5l,sizeof(int),1,f);
	g_hash_table_foreach(hash_table,(GHFunc) ht_to_binary,f);
	fclose(f);
	printf("Filtragem bruta salva em %s\n",outname);
	printString("Filtragem bruta salva em",outname);
	return;
}

GHashTable* read_binary_to_ht(FILE *f){
	GHashTable *hash_table;
	GHashTable *sub_hash_table;
	int i;
	int size;
	int seq_len;
	int qsenso;
	int qasenso;
	float qnt_relativa;
	int sub_size;
	int sub_seq_len;
	int sub_qsenso;
	int sub_qasenso;
	int sub_status;
	float sub_qnt_relativa;
	char *esq_seq;
	char *seq;
	gboolean regiao5l;
	
	hash_table = criar_ghash_table();
	
	fread(&size,sizeof(int),1,f);
	fread(&regiao5l,sizeof(int),1,f);
	for(i=0;i<size;i++){		
		fread(&seq_len,sizeof(int),1,f);
		seq = (char*)malloc(seq_len*sizeof(char));
		fread(seq,sizeof(char),seq_len,f);
		fread(&qsenso,sizeof(int),1,f);
		fread(&qasenso,sizeof(int),1,f);
		if(regiao5l){
			fread(&sub_status,sizeof(int),1,f);
			sub_hash_table = criar_ghash_table();
			while(sub_status){
				fread(&seq_len,sizeof(int),1,f);
				esq_seq = (char*)malloc(seq_len*sizeof(char));
				fread(esq_seq,sizeof(char),seq_len,f);
				fread(&sub_qsenso,sizeof(int),1,f);
				fread(&sub_qasenso,sizeof(int),1,f);
				adicionar_ht(sub_hash_table,esq_seq,NULL,criar_value(0,sub_qsenso,sub_qasenso,0));
				fread(&sub_status,sizeof(int),1,f);
			}
		}
		adicionar_ht(hash_table,seq,NULL,criar_value(0,qsenso,qasenso,0));
	}
	
	return hash_table;
}

int tamanho_ht(GHashTable *hash_table){
	return g_hash_table_size(hash_table);
}
