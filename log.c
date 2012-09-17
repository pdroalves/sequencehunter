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
#include "estruturas.h"
void prepareLog();
void print_time();
void printString(char*,char*);
void printSet(int);
void print_matchs(int,int);
void print_tempo(float);
void closeLog();


FILE *logfile;
FILE *logfileDetalhado;

void prepareLog(){
  //Abre e prepara arquivo log.dat para receber mensagens de log
  
  logfile = fopen("log.dat","a");
  logfileDetalhado = fopen("logDetalhado.dat","a");
  
	if(ferror(logfile) != 0 || ferror(logfileDetalhado) !=0){
		printf("Erro! Impossível salvar log\n");
		exit(1);
	}
   
  fprintf(logfile,"\n\n-------------------------\n");
  fprintf(logfileDetalhado,"\n\n-------------------------\n");
 
  print_time();
}

void print_time(){
 
 struct tm *local;
 time_t t;
 
 t = time(NULL);
 local = localtime(&t);
 
 fprintf(logfile,"%s\n",asctime(local));
 fprintf(logfileDetalhado,"%s\n\n",asctime(local));

  return;
}

//Métodos específicos#######

void print_open_file(char *c){
	fprintf(logfile,"Arquivo %s aberto.",c);
	fprintf(logfileDetalhado,"Arquivo %s aberto.",c);
	return;
}
void printString(char *c,char *s){
	if(s != NULL)
		fprintf(logfile,"%s %s\n",c,s);
	else
		fprintf(logfile,"%s\n",c);
	
}

void printSet(int n){
	fprintf(logfile,"Bases por sequência: %d.\n",n);
	fprintf(logfileDetalhado,"Bases por sequência: %d.\n",n);
}

void print_seqs_carregadas(int n){
	fprintf(logfileDetalhado,"Sequências carregadas: %d\n",n);
}

void print_total_seqs(int sensos,int antisensos){
	fprintf(logfile,"Sequências senso encontradas: %d.\nSequências antisenso encontradas: %d.\n",sensos,antisensos);
	fprintf(logfileDetalhado,"Sequências senso encontradas: %d.\nSequências antisenso encontradas: %d.\n",sensos,antisensos);
}

void print_despareadas_seqs(int sensos,int antisensos){
	fprintf(logfile,"Sequências senso despareadas: %d.\nSequências antisenso despareadas: %d.\n",sensos,antisensos);
	fprintf(logfileDetalhado,"Sequências senso despareadas: %d.\nSequências antisenso despareadas: %d.\n",sensos,antisensos);
}

void print_despareadas(char *seq,int sensos,int antisensos){
	fprintf(logfile,"	%s - S:%d - As:%d.\n",seq,sensos,antisensos);
	fprintf(logfileDetalhado,"	%s - S:%d - As:%d.\n",seq,sensos,antisensos);
}

void print_tipos_seqs(int sensos,int antisensos){
	fprintf(logfile,"Tipos de sensos encontradas: %d.\nTipos de antisensos encontradas: %d.",sensos,antisensos);
	fprintf(logfileDetalhado,"Tipos de sensos encontradas: %d.\nTipos de antisensos encontradas: %d.",sensos,antisensos);	
}

void print_resultados(lista_ligada** resultados){
	int i;
	i = 0;
	
	fprintf(logfile,"Pares encontrados:\n\n");
	while(resultados[i]->pares != -1){
		if(resultados[i]->pares != 0){
			fprintf(logfile,"	%s x%d => %.3f %\n",resultados[i]->senso,resultados[i]->pares,resultados[i]->qnt_relativa*100);
			fprintf(logfileDetalhado,"	%s x%d => %.3f %\n",resultados[i]->senso,resultados[i]->pares,resultados[i]->qnt_relativa*100);
		}
			i++;
	}
	
}

void print_tempo(float tempo){
	if(tempo > 0.5)
		fprintf(logfile,"Tempo decorrido: %fs\n",tempo/1000.0);
	else
		fprintf(logfile,"Tempo decorrido: %fms\n",tempo);
		
		fprintf(logfileDetalhado,"Tempo decorrido: %fms\n",tempo);
	
}

void print_sequencias_validas(int seqs_validas){
	if(seqs_validas >= 0){
		printf("Sequências válidas encontradas: %d.\nAVISO: Sequências de tamanho variável.",-seqs_validas);
	}else{	
		printf("Sequências válidas encontradas: %d.\n",seqs_validas);
	}
}

//##########################

void closeLog(){

  print_time();
  fprintf(logfile,"\n-------------------------\n");
  
  
   if(logfile != NULL)
      fclose(logfile);
   if(logfileDetalhado != NULL)
		fclose(logfileDetalhado);
	return;
}

