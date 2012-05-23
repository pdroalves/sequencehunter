#include <stdio.h>
#include "estruturas.h"
#include "log.h"

#define TAM_MAX 10000

int open_file(char**,int);
void close_file();
int check_seq(char *seq,int *bloco1,int *bloco2,int *blocoV);
void prepare_buffer(Buffer*,int c);
void fill_buffer(Buffer*,int);

FILE **f;
int files = 0;
/* converts integer into string */

char* itoa(unsigned long num) {
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
	while(i < tam && tmp == -1){
		if(seq[i] != 'N')
			tmp = i;
		i++;	
	}
	*bloco2 = tam - tmp;
	*blocoV = tam - *bloco1 - *bloco2;
	while(i < tam && tmp != -1){
		if(seq[i] == 'N')
			inv = i;
		i++;
	}			
	
	if(*bloco1 == -1 || *bloco2 == -1 || inv != -1)
		return 0;
	return 1;
}

int open_file(char **entrada,int qnt){
	int checks[qnt];
	int i;
	int abertos = 0;
	int tmp = 0;
	f = (FILE**)malloc(qnt*sizeof(FILE*));
	while(files < qnt && abertos+1 < qnt){
		f[files] = fopen(entrada[abertos+1],"r+");
		checks[files] = f[files]!=NULL;
		if(checks[files] == 0){
			printf("Arquivo %s não pode ser aberto.\n",entrada[files+1]);
			abertos++;
		}else{
			printf("Arquivo %s aberto.\n",entrada[abertos+1]);
			print_open_file(entrada[abertos+1]);
			files++;
			abertos++;
		}
	}
	for(i=0;i<qnt;i++) tmp += checks[files];
	return tmp==qnt;
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
	rewind(f[0]);
	*n = (int)(strlen(tmp));
	free(tmp);
	return;
}

void prepare_buffer(Buffer *b,int c){
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	b->load = 0;
	printString("Buffer configurado para: ",itoa(c));
	return;
}

void fill_buffer(Buffer *b,int n){
	int i = 0;
	int j = 0;
	for(j=0;j < files && i < b->capacidade;j++){		
		while(i < b->capacidade && feof(f[j]) == 0){
				b->seq[i] = (char*)malloc((n+1)*sizeof(char));
				fscanf(f[j],"%s",b->seq[i]);
				strcat(b->seq[i],"\0");
				i++;
		}
		b->load = i;	
		if(i < b->capacidade && i!=0){ 
			b->load--;
			i = b->load;
		}
		if(feof(f[files-1]) == 1 && b->load ==0) b->load = -1;//Não há mais arquivos
	}
	return;
}

