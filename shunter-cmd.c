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
	#include "version.h"
	#define SEQ_BUSCA_TAM 1000

	gboolean fromFile = FALSE;
	gboolean disable_cuda = FALSE;
	gboolean verbose = FALSE;
	gboolean silent = FALSE;
	gboolean check_build = FALSE;
	//###############
	static GOptionEntry entries[] = 
	  {
		//O comando "rápido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
		//Entrada de posicoes
		{ "disablecuda", 'd', 0, G_OPTION_ARG_NONE, &disable_cuda, "Impede o processamento através da arquitetura CUDA", NULL },
		{ "fromFile", 'f', 0, G_OPTION_ARG_NONE, &fromFile, "Carrega a configuração de busca do arquivo shset.dat", NULL },
		{ "check", 'c', 0, G_OPTION_ARG_NONE, &check_seqs, "Verifica a biblioteca antes de executar a busca", NULL },
		{ "silent", 's', 0, G_OPTION_ARG_NONE, &silent, "Execução silenciosa", NULL },
		{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
		{ "build", 'b', 0, G_OPTION_ARG_NONE, &check_build, "Retorna o número da build", NULL },
		
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
	  int err;
	  int seqs_validas;
	  int b1_size;
	  int b2_size;
	  int bv_size;
	  int is_cuda_available = 0;
	  int bibliotecas_validas;

	  
	  //##########################
	  //Carrega parametros de entrada
	  context = g_option_context_new ("Main application");
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

	  printf("Iniciando Sequence Hunter...\n\n",get_build());
	  if(verbose)
		printf("Modo verbose\n");
	  
	  //Inicializa
	  prepareLog();	 
	  
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
	  bibliotecas_validas = open_file(argv,argc);
	if(bibliotecas_validas == 0){
		printf("Por favor, entre uma biblioteca válida.\n");
		exit(1);
	}
	  seqs_validas = check_sequencias_validas();
	  
	//////////////////////////////////
	////////////////////////////////////////////////////////
	if(fromFile){
		FILE *set;
		set = fopen("shset.dat","r");
		if(set == NULL){
			printf("Arquivo shset.dat não encontrado.\n");
			exit(1);
		}
		fscanf(set,"%s",c);
		if(c == NULL){
			printf("Erro na leitura\n");
			exit(1);
		}
		fscanf(set,"%s",nome);
		
	}else{
	  printf("Entre a sequência: ");
	  scanf("%s",c);
	  if(c == NULL){
		  printf("Erro na leitura\n");
		  exit(1);
	  }

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
	 
	if(disable_cuda){
		printf("Forçando modo OpenMP.\n");
		printString(NULL,"Forçando modo OpenMP.");
		aux(0,c,b1_size,b2_size,c_size,disable_cuda,silent,verbose); 
	}
	else aux(is_cuda_available,c,b1_size,b2_size,c_size,disable_cuda,silent,verbose);
	processar(bv_size);
	  
	  if(!silent)
		printf("Algoritmo concluído.\n");
	 close_file();
	 free(c);
	 
	return 0;
	}
