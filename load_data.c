#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "estruturas.h"
#include "log.h"
#include "processing_data.h"

#define TAM_MAX 10000

void get_setup(int*);	
void fill_buffer(struct buffer*,int);
int check_seq(char*,int*,int*,int*);
void prepare_buffer(struct buffer*,int);
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
	int i;
	int abertos = 0;
	int tmp = 0;
	int seqs_validas;

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
	get_setup(&n);
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	b->resultado = (int*)malloc(c*sizeof(int));
	
	for(i=0;i<c;i++) b->seq[i] = (char*)malloc((n+1)*sizeof(char));
	printf("Buffer configurado para sequências de até %d posições.\n",n);
	b->load = 0;
	printString("Buffer configurado para: ",itoaa(c));
	return;
}

void fill_buffer(Buffer *b,int n){
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
	//if(hold != NULL)
	//free(hold);
	return;
}

