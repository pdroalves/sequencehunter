	//      shunter-cmd.cu
	//      
	//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
	//      
	//		Sequence Hunter 
	//		Execução via linha de comando
	//
	//		27/03/2012

	#include <stdio.h>
	#include <stdlib.h>
	#include <cuda.h>
	#include <cuda_runtime_api.h>
	#include <glib.h>
	#include "operacoes.h"
	#include "cuda_functions.h"
	//#include "linkedlist.h"
	#include "estruturas.h"
	#include "go_hunter.h"
	#include "log.h"
	#include "load_data.h"
	#include "processing_data.h"
	#include "linkedlist.h"
	#include "version.h"
	#define SEQ_BUSCA_TAM 1000

	gchar *fromFile;
	gboolean disable_cuda = FALSE;
	gboolean verbose = FALSE;
	gboolean silent = FALSE;
	gboolean check_build = FALSE;
	gboolean just_process = FALSE;
	gboolean debug = FALSE;
	gboolean cutmode = FALSE;
	gboolean keep = FALSE;
	gboolean gui_run = FALSE;
	gint max_events = 20;
	//###############
	static GOptionEntry entries[] = 
	  {
		//O comando "rápido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
		//Entrada de posicoes
		{ "disablecuda", 'd', 0, G_OPTION_ARG_NONE, &disable_cuda, "Impede o processamento atraves da arquitetura CUDA.", NULL },
		{ "fromFile", 'f', 0, G_OPTION_ARG_STRING, &fromFile, "Carrega a configuracao de busca do arquivo shset.dat.", NULL },
		{ "check", 'c', 0, G_OPTION_ARG_NONE, &check_seqs, "Verifica a biblioteca antes de executar a busca.", NULL },
		{ "events", 'e', 0, G_OPTION_ARG_INT, &max_events, "Quantidade maxima de eventos a serem exportados. Padrao: 20.", NULL},
		{ "cutseqs", 't', 0, G_OPTION_ARG_NONE, &cutmode, "Guarda apenas o bloco variavel central de cada sequencia apos a filtragem.", NULL },
		{ "keep", 'k', 0, G_OPTION_ARG_NONE, &keep, "Gera binario com os dados brutos apos filtragem.", NULL },
		{ "silent", 's', 0, G_OPTION_ARG_NONE, &silent, "Execucao silenciosa.", NULL },
		{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose.", NULL },
		{ "build", 'b', 0, G_OPTION_ARG_NONE, &check_build, "Retorna o numero da build.", NULL },
		{ "process", 'p', 0, G_OPTION_ARG_NONE, &just_process, "Carrega e apenas processa dados já existentes.", NULL },
		{ "debug", NULL, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &debug, "Modo para debug.", NULL },		
		{ "gui", NULL, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &gui_run, NULL, NULL },		
		{ NULL }
	  };

	int get_build(){
		
		return build; 
	}


	//####################
	int main (int argc,char *argv[]) {

	  GError *error = NULL;
	  GOptionContext *context;
	  char *c;
	  char *nome;
	  int c_size;
	  int seqs_validas;
	  int b1_size;
	  int b2_size;
	  int bv_size;
	  int is_cuda_available = 1;
	  int bibliotecas_validas;
	  lista_ligada *resultados;
	  Params set;
	  GHashTable* hash_table;

	  
	  //##########################
	  //Carrega parametros de entrada
	  context = g_option_context_new ("library_file_path");
	  g_option_context_add_main_entries (context, entries,NULL);
	  if (!g_option_context_parse (context, &argc, &argv, &error))
		{
		  g_print ("option parsing failed: %s\n", error->message);
		  exit (1);
		}  
	  //###########################  
	  
	  if(check_build){
		  printf("Build: %d\n",get_build());
		  return 0;
	  }
	  if(!silent)
		printf("Iniciando Sequence Hunter...\n\n",get_build());
	  if(verbose && !silent)
		printf("Modo verbose\n");
	  
	  //Inicializa
	  prepareLog();	 
	  
	  if(just_process){
		if(!silent || gui_run)
			printf("Iniciando em modo de processamento...\n");
		  FILE *f;
		  f = fopen(argv[1],"r");
		  hash_table = read_binary_to_ht(f);
	  }else{
	  
		  c = (char*)malloc((SEQ_BUSCA_TAM+1)*sizeof(char));
		  nome = (char*)malloc((100)*sizeof(char));
		  
		  if(c == NULL){
			  printf("Erro alocando memória.\n");
			  exit(1);
		  }
		 
		////////////////////////////////////////////////////////
		////////////////// Abre arquivos de bibliotecas/////////
		////////////////////////////////////////////////////////
		if(argc == 1){
			printf("Por favor, entre uma biblioteca válida.\n");
			exit(1);
		}
		  bibliotecas_validas = open_file(argv,argc,silent);
		if(bibliotecas_validas == 0){
			printf("Por favor, entre uma biblioteca válida.\n");
			exit(1);
		}
		  seqs_validas = check_sequencias_validas(silent);
		  
		//////////////////////////////////
		////////////////////////////////////////////////////////
		if(fromFile != NULL){
			FILE *set;
			set = fopen(fromFile,"r");
			if(set == NULL){
				printf("Arquivo %s não encontrado.\n",fromFile);
				exit(1);
			}
			if(!silent)
				printf("Configuração de busca recuperada de %s\n",fromFile);
			fscanf(set,"%s",c);
			if(c == NULL){
				printf("Erro na leitura\n");
				exit(1);
			}
			fscanf(set,"%s",nome);
			
		}else{
			if(!silent)
		  printf("Entre a sequência: ");
		  scanf("%s",c);
		  if(c == NULL){
			  printf("Erro na leitura\n");
			  exit(1);
		  }

	  if(!silent)
		printf("Entre uma identificação para essa busca: ");
		scanf("%s",nome);
		}
		
		 if(!check_seq(c,&b1_size,&b2_size,&bv_size)){
			 printf("Sequência de busca inválida\n");
			 exit(1);
		}  
		printString("Identificação da busca: ",nome);
		  printString("Sequência de busca: ",c);
		  
		 c_size = b1_size+b2_size+bv_size;
		 
		 //Guarda parametros
		 set.verbose = verbose;
		 set.silent = silent;
		 set.debug = debug;
		 set.cut_central = cutmode;
		 set.gui_run = gui_run;
		 
		if(disable_cuda){
	  if(!silent || gui_run)
			printf("Forçando modo OpenMP.\n");
			printString(NULL,"Forçando modo OpenMP.");
			hash_table = aux(0,c,b1_size,b2_size,c_size,set); 
		}
		else{
			hash_table = aux(is_cuda_available,c,b1_size,b2_size,c_size,set);
		}
		free(c);
	}
	
	#pragma omp parallel num_threads(2) shared(hash_table) shared(bv_size) shared(max_events) shared(silent)
	{
		#pragma omp sections
		{
			#pragma omp section
			{
			if(!just_process && keep)
				write_ht_to_binary(hash_table);			
			}
			#pragma omp section
			{
				resultados = processar(hash_table,bv_size,max_events,silent,gui_run);
			}
		}
	}
	
	imprimir(resultados,max_events,silent,gui_run);
	
	if(!silent)
		printf("Algoritmo concluído.\n");
	close_file();
	if(hash_table != NULL)
		destroir_ghash_table(hash_table);
	return 0;
}
