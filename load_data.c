#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "cuda.h"
#include "cuda_runtime_api.h"
#include "estruturas.h"
#include "log.h"
#include "processing_data.h"

#define TAM_MAX 10000

void get_setup(int*);	
int check_seq(char*,int*,int*,int*);
void close_file();
int open_file(char **entrada,int);

FILE **f;
int files = 0;
gboolean check_seqs = FALSE;
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
int open_file(char **entrada,int qnt){
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
			abertos++;
		}else{
			printf("Arquivo %s aberto.\n",entrada[abertos+1]);
			print_open_file(entrada[abertos+1]);
			files++;
			abertos++;
		}
	}
	free(checks);
	return files;
}

int check_sequencias_validas(){
	int seqs_validas = 0;
	if(check_seqs && files > 0){
		seqs_validas = get_sequencias_validas(f,files);
		if(seqs_validas >=0)
			printf("Sequencias validas encontradas: %d\n",seqs_validas);
		else
			printf("Sequencias validas encontradas: %d\nATENÇÃO: Sequências de tamanho variável.\n",-seqs_validas);
		print_seqs_carregadas(seqs_validas);
	}
	return seqs_validas;
}

void close_file(){
	int i;
	for(i=0;i<files;i++)
		fclose(f[i]);
	return;
}

void get_setup(int *n){
	char *tmp;
	
	//Suponho que todas as sequências nas bibliotecas tem o mesmo tamanho
	tmp = (char*)malloc(TAM_MAX*sizeof(char));
	fscanf(f[0],"%s",tmp);
	
	while(!check_seq_valida(tmp)) fscanf(f[0],"%s",tmp);		
	rewind(f[0]);
	*n = (int)(strlen(tmp));
	
	free(tmp);
	return;
}

void prepare_buffer(Buffer *b,int c){
	int i;
	int n;
	char* tamanho_do_buffer;
	
	get_setup(&n);
	
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	for(i=0;i<c;i++) b->seq[i] = (char*)malloc((n+1)*sizeof(char));
	
	printf("Buffer configurado para sequências de até %d posições.\n",n);
	b->load = 0;
	tamanho_do_buffer = itoaa(c);
	printString("Buffer configurado para: ",tamanho_do_buffer);
	
	free(tamanho_do_buffer);
	return;
}

void prepare_buffer_CUDA(Buffer *d_buffer,int n,int c){
	int i;
	char *tamanho_do_buffer;
	
	d_buffer->seq = (char**)malloc(c*sizeof(char*));

	for(i=0;i<c;i++) 
		cudaMalloc((void**)&(d_buffer->seq[i]),(n+1)*sizeof(char));
    
	printf("Buffer configurado para sequências de até %d posições.\n",n);
    
	tamanho_do_buffer = itoaa(c);
	printString("Buffer configurado para: ",tamanho_do_buffer);
	
	free(tamanho_do_buffer);
	return;
}

void fill_buffer(Buffer *b){
	int i = 0;
	int j = 0;
	char *hold;
	
	hold = (char*)malloc(TAM_MAX*sizeof(char));
	//Enche buffer
	for(j=0;j < files && i < b->capacidade;j++){		
		while(i < b->capacidade && !feof(f[j])){
				fscanf(f[j],"%s",hold);
				if(check_seq_valida(hold)){
					strcpy(b->seq[i],hold);
					i++;
				}
		}
		b->load = i;	
		if(i < b->capacidade && i!=0){ 
			b->load--;
			i = b->load;
		}
		if(feof(f[files-1]) && b->load == 0) b->load = -1;//Não há mais arquivos
	}
	free(hold);
	return;
}

int fill_buffer_CUDA(char **seqs,int MAX_TO_LOAD){
	int i = 0;
	int j = 0;
	char *hold;
	
	hold = (char*)malloc(TAM_MAX*sizeof(char));
	
	//Enche buffer
	for(j=0;j < files && i < MAX_TO_LOAD;j++){		
		while(i < MAX_TO_LOAD && !feof(f[j])){
				fscanf(f[j],"%s",hold);
				if(check_seq_valida(hold)){
					//printf("Lido: %s\n",hold);
					strcpy(seqs[i],hold);
					strcat(seqs[i],"\0");
					i++;
				}
		}
		if(i < MAX_TO_LOAD && i!=0){ 
			i--;
		}
		if(feof(f[files-1]) && i == 0) i = -1;//Não ha mais arquivos
	}
	free(hold);
	return i;
}

void despejar_seq(char *seq,FILE *f){
	if(seq != NULL){
			fputs(seq,f);
			free(seq);
		}
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

