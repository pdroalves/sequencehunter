#include <stdio.h>
#include "estruturas.h"
#include "log.h"

int open_file(char*);
void close_file();
void prepare_buffer(Buffer*);
void fill_buffer(Buffer*);
int check_file_end();

FILE *f;

int open_file(char *entrada){
	f = fopen(entrada,"r+");
	return f!=NULL;
}

void close_file(){
	fclose(f);
}

void get_setup(int *m,int *n){
	fscanf(f,"%d %d",m,n);
	fgets(NULL,0,f);
}

void prepare_buffer(Buffer *b,int c){
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	printString("Buffer configurado para: ","2");
}

void fill_buffer(Buffer *b,int n){
	int i;

	for(i=0;i<b->capacidade && feof(f) == 0;i++){
		b->seq[i] = (char*)malloc((n+1)*sizeof(char));
		fscanf(f,"%s",b->seq[i]);
		strcat(b->seq[i],"\0");
	}	
	
}

int check_file_end(){
	return feof(f);
}

