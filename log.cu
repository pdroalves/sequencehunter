#include <stdio.h>
#include <stdlib.h>
#include <time.h>
FILE *logfile;
//FILE *logfileDetalhado;

void prepareLog();
void print_time();
void printToLog_Fim(int tiro,int raio,int fim,float x,float z);
void printToLog_Mem(float MemUsada,float MemPorIter);
void printToLog_Runtime(float t);
void printString(char*,char*);
void printSet(int,int);
void print_matchs(int,int);
void closeLog();

void prepareLog(){
  //Abre e prepara arquivo log.dat para receber mensagens de log
  
  logfile = fopen("log.dat","a");
  //logfileDetalhado = fopen("logDetalhado.dat","a");
  
  //if(ferror(logfile) != 0 || ferror(logfileDetalhado) != 0){
//  	printf("Erro! Impossível salvar log\n");
 // 	exit(1);
 // }
   
  fprintf(logfile,"-------------------------\n");
 // fprintf(logfileDetalhado,"-------------------------\n");
  print_time();
}

void print_time()
{
 
 struct tm *local;
 time_t t;
 
 t = time(NULL);
 local = localtime(&t);
 
 fprintf(logfile,"%s\n\n",asctime(local));
 //fprintf(logfileDetalhado,"%s\n\n",asctime(local));

  return;
}

//Métodos específicos#######
void printString(char *c,char *s){
	fprintf(logfile,"%s %s\n",c,s);
}

void printSet(int m,int n){
	fprintf(logfile,"Sequências: %d.\nBases por sequência: %d.\n",m,n);
}

void print_seqs_carregadas(int n){
	fprintf(logfile,"Sequências carregadas: %d\n",n);
}

void print_matchs(int sensos,int antisensos){
	fprintf(logfile,"Sequências senso encontradas: %d.\nSequências antisenso encontradas: %d.",sensos,antisensos);
}
//##########################

void printToLog_Runtime(float t){
	fprintf(logfile,"\nTempo de execução: %f ms.\n",t);
	//fprintf(logfileDetalhado,"\nTempo de execução: %f ms.\n",t);
}

void closeLog(){
   if(logfile != NULL)
      fclose(logfile);
  // if(logfileDetalhado != NULL)
//		fclose(logfileDetalhado);
	return;
}

