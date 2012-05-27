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
	//#include "operacoes.h"
	//#include "linkedlist.h"
	#include "estruturas.h"
	#include "aux.h"
	#include "log.h"
	#include "load_data.h"
	#include "pilha.h"
	#include "processing_data.h"
	#define SEQ_BUSCA_TAM 1000

	//###############
	static GOptionEntry entries[] = 
	  {
		//O comando "rápido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
		//Entrada de posicoes
		{ "disablecuda", 'd', 0, G_OPTION_ARG_NONE, &disable_cuda, "Impede o processamento através da arquitetura CUDA", NULL },
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
	  
	  char *c;
	  int c_size;
	  int err;
	  int b1_size;
	  int b2_size;
	  int bv_size;
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
	 //////////////////////////////////
	////////////////////////////////////////////////////////
	  
	  printf("Entre a sequência: ");
	  scanf("%s",c);
	  if(c == NULL){
		  printf("Erro na leitura\n");
		  exit(1);
	  }
	  
	 if(!check_seq(c,&b1_size,&b2_size,&bv_size)){
		 printf("Sequência de busca inválida\n");
		 exit(1);
	}  
	  printString("Sequência de busca: ",c);
	  
	 c_size = b1_size+b2_size+bv_size;
	  
	if(disable_cuda){
		printf("Forçando modo OpenMP.\n");
		printString(NULL,"Forçando modo OpenMP.");
		aux(0,c,b1_size,b2_size,c_size,&p_sensos,&p_antisensos); 
	}
	else aux(check_gpu_mode(),c,b1_size,b2_size,c_size,&p_sensos,&p_antisensos);
	processar(&p_sensos,&p_antisensos);
	  
	 close_file();
	 free(c);
	destroy(&p_sensos);
	destroy(&p_antisensos);
	return 0;
	}
