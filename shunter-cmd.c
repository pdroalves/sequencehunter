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
	#include <string.h>
	#include "operacoes.h"
	#include "cuda_functions.h"
	//#include "linkedlist.h"
	#include "estruturas.h"
	#include "aux.h"
	#include "log.h"
	#include "load_data.h"
	#include "pilha.h"
	#include "processing_data.h"
	#define SEQ_BUSCA_TAM 10000

	//###############
	static gboolean disable_cuda = 0;
	static gboolean silent = 0;
	static gboolean verbose = 0;
	static gchar *seqname = NULL;
	static gchar *seq = NULL;
	static GOptionEntry entries[] = 
	  {
		//O comando "rápido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
		//Entrada de posicoes
		{ "seqname", 'n', 0, G_OPTION_ARG_STRING, &seqname, "Define nome da sequencia", NULL },
		{ "seq", 'q', 0, G_OPTION_ARG_STRING, &seq, "Define sequencia", NULL },
		{ "disablecuda", 'd', 0, G_OPTION_ARG_NONE, &disable_cuda, "Impede o processamento através da arquitetura CUDA", NULL },
		{ "check", 'c', 0, G_OPTION_ARG_NONE, &check_seqs, "Verifica a biblioteca antes de executar a busca", NULL },
		{ "silent", 's', 0, G_OPTION_ARG_NONE, &silent, "Execução silenciosa", NULL },
		{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
		{ NULL }
	  };



	//####################
	int main (int argc,char *argv[]) {
		
	  //###########################  
	  GError *error = NULL;
	  GOptionContext *context;

	  context = g_option_context_new (NULL);
	  g_option_context_add_main_entries (context, entries,NULL);
	  if (!g_option_context_parse (context, &argc, &argv, &error))
		{
		  g_print ("option parsing failed: %s\n", error->message);
		  exit (1);
		}  
	  //##########################
	  
	  int c_size;
	  int err;
	  int seqs_validas;
	  int b1_size;
	  int b2_size;
	  int bv_size;
	  char *c;
	  int is_cuda_available = 0;
	  pilha p_sensos;
	  pilha p_antisensos;
	  
	  
	  //Inicializa
	  prepareLog();
	  p_sensos = criar_pilha();
	  p_antisensos = criar_pilha();
	 
	  
	  c = (char*)malloc((SEQ_BUSCA_TAM+1)*sizeof(char));
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
	  open_file(argv,argc);
	  seqs_validas = check_sequencias_validas();
	   
	 //////////////////////////////////
	////////////////////////////////////////////////////////
	  if(strlen(c) == 0){
		  printf("Entre a sequência: ");
		  scanf("%s",c);
		  if(c == NULL){
			  printf("Erro na leitura\n");
			  exit(1);
		  }
		}
	 if(!check_seq(c,&b1_size,&b2_size,&bv_size)){
		 printf("Sequência de busca inválida\n");
		 exit(1);
	}  
	  printString("Sequência de busca: ",c);
	  
	 c_size = strlen(c);
	  
	if(disable_cuda){
		printf("Forçando modo OpenMP.\n");
		printString(NULL,"Forçando modo OpenMP.");
		aux(0,c,b1_size,b2_size,c_size,&p_sensos,&p_antisensos,disable_cuda,silent,verbose); 
	}
	else aux(is_cuda_available,c,b1_size,b2_size,c_size,&p_sensos,&p_antisensos,disable_cuda,silent,verbose);
	processar(&p_sensos,&p_antisensos);
	 
	 closeLog(); 
	 close_file();
	 free(c);
	 
	return 0;
	}
