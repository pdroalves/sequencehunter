#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "../Headers/estruturas.h"
#include "../Headers/log.h"
#include "../Headers/processing_data.h"

#define TAM_MAX 10000

int get_setup();
int check_seq(char*,int*,int*,int*);
void close_file();
int open_file(char **entrada,int qnt,gboolean silent);

FILE **f;
int files = 0;
gboolean check_seqs = FALSE;
char *hold;
/* converts integer into string */

char* itoaa(unsigned long num) {
        char* retstr = (char*)calloc(12, sizeof(char));
        if (sprintf(retstr, "%ld", num) > 0) {
                return retstr;
        } else {
                return NULL;
        }
}

int check_seq(char *seq,int *bloco1,int *bloco2,int *blocoV){
	int i;
	int tam;
	int tmp;
	int inv;
	
	*bloco1 = *bloco2 = *blocoV = -1;
	i = 0;
	tam = strlen(seq);
	inv = tmp = -1;
	
	while(i < tam && *bloco1 == -1){
		if(seq[i] == 'N')
			*bloco1 = i;
		i++;	
	}
	while(i <= tam && *blocoV == -1){
		if(seq[i] != 'N')
			*blocoV = i - *bloco1;
		i++;	
	}
	*bloco2 = tam - *bloco1 - *blocoV;
		
	
	if(*bloco1 == -1 || *bloco2 == -1 || *blocoV == -1)
		return 0;
	return 1;
}
int open_file(char **entrada,int qnt,gboolean silent){
	int *checks;
	int abertos = 0;
	int tmp = 0;

	checks = (int*)malloc(qnt*sizeof(int));
	f = (FILE**)malloc(qnt*sizeof(FILE*));
	
	while(files < qnt && abertos+1 < qnt){
		f[files] = fopen(entrada[abertos+1],"r");
		checks[files] = f[files]!=NULL;

		if(!checks[files]){
			printf("Arquivo %s não pode ser aberto.\n",entrada[files+1]);
			printString("Arquivo não pode ser aberto: ",entrada[files+1]);
			files++;
		}else{
			printf("Arquivo %s aberto.\n",entrada[abertos+1]);
			print_open_file(entrada[abertos+1]);
			files++;
			abertos++;
		}
	}
	free(checks);
	return abertos;
}

int check_sequencias_validas(gboolean silent){
	int seqs_validas = 0;
	if(check_seqs && files > 0){
		seqs_validas = get_sequencias_validas(f,files);
	  if(!silent){
			if(seqs_validas >=0)
				printf("Sequencias validas encontradas: %d\n",seqs_validas);
			else
				printf("Sequencias validas encontradas: %d\nATENÇÃO: Sequências de tamanho variável.\n",-seqs_validas);
		}
	}
	return seqs_validas;
}

void close_file(){
	int i;
	for(i=0;i<files;i++)
		fclose(f[i]);
	return;
}

int get_setup(){
	char *tmp;
	int n;
	
	//Suponho que todas as sequências nas bibliotecas tem o mesmo tamanho
	tmp = (char*)malloc(TAM_MAX*sizeof(char));
	fscanf(f[0],"%s",tmp);
	
	while(!check_seq_valida(tmp)) 
		fscanf(f[0],"%s",tmp);		
	
	rewind(f[0]);
	n = (int)(strlen(tmp));
	
	
	free(tmp);
	return n;
}

void prepare_buffer(Buffer *b,int c){
	int i;
	int n;
	char* tamanho_do_buffer;
	
	n = get_setup();
	
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	hold = (char*)malloc(MAX_SEQ_SIZE*sizeof(char));	
	
	for(i=0;i<c;i++) 
		b->seq[i] = (char*)malloc((n+1)*sizeof(char));
	
	b->load = 0;
	tamanho_do_buffer = itoaa(c);
	printString("Buffer configurado para: ",tamanho_do_buffer);
	
	free(tamanho_do_buffer);
	return;
}

void prepare_buffer_cuda(){
	hold = (char*)malloc(MAX_SEQ_SIZE*sizeof(char));
}

void fill_buffer(char **seqs,int MAX_TO_LOAD,int *SEQS_LOADED){
	int i = 0;
	int j = 0;
	
	//Enche buffer
	for(j=0;j < files && i < MAX_TO_LOAD;j++){		
		while(i < MAX_TO_LOAD && !feof(f[j])){
			// Le sequencia, verifica se eh valida e incrementa a contagem
			fscanf(f[j],"%s",hold);
			fscanf(f[j],"%s",hold);
			fscanf(f[j],"%s",hold);
			if(seqs[i] == NULL)
				seqs[i] = (char*)malloc((strlen(hold)+1)*sizeof(char));
			strcpy(seqs[i],hold);
			fscanf(f[j],"%s",hold);
			fscanf(f[j],"%s",hold);
			i++;
		}
		// Corrige contagem errada
		if(i < MAX_TO_LOAD && i > 0)
			i--;
		if(feof(f[files-1]) && i == 0)
			i = -1;// Nao ha mais arquivos
	}
	
	*SEQS_LOADED = i;
	
	return;
}


char* carrega_do_arquivo(int n,FILE *filename){
	char *seq;
	#pragma omp critical
	{
		if(!feof(filename)){
			int retorno;
			seq = (char*)calloc('\0',(n+1)*sizeof(char));
			fgets(seq,n+1,filename);
		}else{
			seq = NULL;
		}
	}
	return seq;
}

void close_buffer(Buffer *b){
	int i;
	/*for(i=0;i<b->capacidade;i++){
		if(b->seq[i] != NULL)
			free(b->seq[i]);
	}*/
	if(hold != NULL)
		free(hold);
}
