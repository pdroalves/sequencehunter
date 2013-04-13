//      log.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções relativas a impressão do log de saída do programa
//
//		30/03/2012

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include <string.h>
#include "../Headers/estruturas.h"

FILE *logfile;
char* logfile_name;

char* get_log_filename(){
	return logfile_name;
}

void print_time(){
 
 time_t t;
 char *tempo;

 time(&t);
 tempo = ctime(&t);
 
 fprintf(logfile,"%s\n",tempo);

  return;
}

void prepareLog(char* output_dir,char *tempo,gboolean gui){
  //Abre e prepara arquivo log.dat para receber mensagens de log
	char tmp[500];	
	int i;	
	
	logfile_name = (char*)malloc(500*sizeof(char));
	if(output_dir != NULL)
		sprintf(tmp,"%sSHunter Log - %s.txt",output_dir,tempo);
	else
		sprintf(tmp,"SHunter Log - %s.txt",tempo);
	for(i=0;i <= strlen(tmp); i++){
		if(tmp[i] == ':' && i > 1)
			logfile_name[i] = ' ';
		else
			logfile_name[i] = tmp[i];
	}

	logfile = fopen(logfile_name,"a");
  
	if(logfile == NULL){
		printf("Erro! Impossível salvar log\n");
		exit(1);
	}
   
	  fprintf(logfile,"\n\n-------------------------\n");

	  print_time();

	  if(gui)
		printf("Log %s\n",logfile_name);

  return;
}

//Métodos específicos#######

void print_open_file(char *c){
	fprintf(logfile,"Arquivo %s aberto.\n",c);
	return;
}

void printStringInt(char *c,int n){
		fprintf(logfile,"%s %d\n",c,n);
}

void printString(char *c,char *s){
	if(s != NULL){
		fprintf(logfile,"%s %s\n",c,s);	
	}else{
		fprintf(logfile,"%s\n",c);
	
	}
}

void printSet(int n){
	fprintf(logfile,"Bases por sequência: %d.\n",n);
}


void print_total_seqs(int sensos,int antisensos){
	fprintf(logfile,"Sequências senso encontradas: %d.\nSequências antisenso encontradas: %d.\n",sensos,antisensos);
}

void print_despareadas_seqs(int sensos,int antisensos){
	fprintf(logfile,"Sequências senso despareadas: %d.\nSequências antisenso despareadas: %d.\n",sensos,antisensos);
}

void print_despareadas(char *seq,int sensos,int antisensos){
	fprintf(logfile,"	%s - S:%d - As:%d.\n",seq,sensos,antisensos);
}

void print_tipos_seqs(int sensos,int antisensos){
	fprintf(logfile,"Tipos de sensos encontradas: %d.\nTipos de antisensos encontradas: %d.",sensos,antisensos);	
}

void print_tempo(float tempo){
	if(tempo > 0.5)
		fprintf(logfile,"Tempo decorrido: %fs\n",tempo/1000.0);
	else
		fprintf(logfile,"Tempo decorrido: %fms\n",tempo);
}

void print_tempo_optional(float tempo){
	if(tempo > 0.5)
		fprintf(logfile,"Tempo decorrido: %fs\n",tempo/1000.0);
	else
		fprintf(logfile,"Tempo decorrido: %fms\n",tempo);	
}

void print_resultado(lista_ligada *p){
	fprintf(logfile,"	%s x%d => %.3f %%, S:%d - AS: %d\n",p->senso,p->pares,p->qnt_relativa*100,p->qsenso,p->qasenso);
	return;
}

void print_string_float(char *s,float f){
	fprintf(logfile,"%s %f",s,f);
	return;
}

void print_seqs_processadas(int total,int sensos,int antisensos){
	fprintf(logfile,"Sequencias processadas: %d - S: %d, AS: %d\n",total,sensos,antisensos);
	return;
}

void print_seqs_filas(int sensos,int antisensos){
	fprintf(logfile,"Filas - S: %d, AS: %d\n\n",sensos,antisensos);
	return;
}

void print_seqs_filas_optional(int sensos,int antisensos){
	fprintf(logfile,"Filas - S: %d, AS: %d\n\n",sensos,antisensos);
	return;
}

//##########################

void closeLog(){

  print_time();
  fprintf(logfile,"\n-------------------------\n");
  if(logfile_name != NULL)
	free(logfile_name);
  
   if(logfile != NULL)
      fclose(logfile);
	return;
}

