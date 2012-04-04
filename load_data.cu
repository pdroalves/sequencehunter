#include <stdio.h>
#include "estruturas.h"
#include "log.h"

int open_file(char*);
void close_file();
void prepare_buffer(Buffer*);
void fill_buffer(Buffer*,int);

FILE *f;

int open_file(char *entrada){
	f = fopen(entrada,"r+");
	return f!=NULL;
}

void close_file(){
	fclose(f);
}

void get_setup(int *n){
	fscanf(f,"%d",n);
	fgets(NULL,0,f);
}

void prepare_buffer(Buffer *b,int c){
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	b->load = 0;
	printString("Buffer configurado para: ","2");
}

void fill_buffer(Buffer *b,int n){
	int i;
	
	for(i=0;i < b->capacidade && feof(f) == 0;i++){
		b->seq[i] = (char*)malloc((n+1)*sizeof(char));
		fscanf(f,"%s",b->seq[i]);
		strcat(b->seq[i],"\0");
		b->load++;
	}
	b->load--;

	if(feof(f) == 1 && b->load == 0) b->load = -1;//Arquivo acabou
	
	return;
}

