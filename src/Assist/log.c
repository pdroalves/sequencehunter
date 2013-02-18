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
void prepareLog();
void print_time();
void printString(char*,char*);
void printSet(int);
void print_tempo(float);
void closeLog();


FILE *logfile;

void prepareLog(char *tempo,gboolean gui){
  //Abre e prepara arquivo log.dat para receber mensagens de log
	char logfile_name[100];
	char logfiledetalhado_name[100];
	char tmp[100];	
	int i;	
	
	tempo[strlen(tempo)-1] = '\0';
	strcpy(tmp,"SHunter Log - ");
	strcat(tmp,tempo);
	strcat(tmp,".txt");

	for(i=0;i <= strlen(tmp); i++){
		if(tmp[i] == ':')
			logfile_name[i] = ' ';
		else
			logfile_name[i] = tmp[i];
	}
  
  logfile = fopen(logfile_name,"a");
  
	if(ferror(logfile) != 0){
		printf("Erro! Impossível salvar log\n");
		exit(1);
	}
   
  fprintf(logfile,"\n\n-------------------------\n");

  print_time();

  if(gui)
	printf("Log %s\n",logfile_name);

  return;
}

void print_time(){
 
 time_t t;
 char *tempo;

 time(&t);
 tempo = ctime(&t);
 
 fprintf(logfile,"%s\n",tempo);

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
  
  
   if(logfile != NULL)
      fclose(logfile);
	return;
}

