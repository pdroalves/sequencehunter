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
#include <time.h>
#include "Headers/operacoes.h"
#include "Headers/cuda_functions.h"
#include "Headers/estruturas.h"
#include "Headers/go_hunter.h"
#include "Headers/log.h"
#include "Headers/load_data.h"
#include "Headers/processing_data.h"
#include "Headers/linkedlist.h"
#include "Headers/version.h"
#include "Headers/database_manager.h"
#include "Headers/database.h"
#ifdef _WIN32
#include <Windows.h>
const char* get_windows_homefolder(){
	const char *homedrive = getenv("HOMEDRIVE");
	const char *homepath = getenv("HOMEPATH");
	char* fullpath;
	fullpath = (char*)malloc((strlen(homedrive)+strlen(homepath)+1)*sizeof(char));
	strcpy(fullpath,homedrive);
	strcat(fullpath,homepath);
	return fullpath;
};
#define DEFAULT_OUTPUT_DIR get_windows_homefolder()
#else
#include <wordexp.h>
const char* expand_tilde(){
	wordexp_t exp_tilde;
	wordexp("~/",&exp_tilde,0);
	return exp_tilde.we_wordv[0];
}
#define DEFAULT_OUTPUT_DIR expand_tilde()
#endif

#define SEQ_BUSCA_TAM 1000

gchar *fromFile;
gchar *target_name;
gchar *target_seq;
gchar *output_dir;
gint dist_regiao_5l = 0;
gint tam_regiao_5l = 0;
gboolean disable_cuda = FALSE;
gboolean verbose = FALSE;
gboolean silent = FALSE;
gboolean check_build = FALSE;
gboolean just_process = FALSE;
gboolean debug = FALSE;
gboolean cutmode = FALSE;
gboolean keep = FALSE;
gboolean gui_run = FALSE;
gboolean regiao5l = FALSE;
gint max_events = 20;
//###############
static GOptionEntry entries[] =
{
		//O comando "rapido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
		//Entrada de posicoes
		{ "target", 'a', 0, G_OPTION_ARG_STRING, &target_seq, "Define a sequencia alvo S.", NULL },
		{ "output", 'o', 0, G_OPTION_ARG_STRING, &output_dir, "Define o diretorio de saída.", NULL },
		{ "name", 'n', 0, G_OPTION_ARG_STRING, &target_name, "Define uma identificacao para a sequencia alvo.", NULL },
		{ "dist5l", NULL, 0, G_OPTION_ARG_INT, &dist_regiao_5l, "Define a quantidade de bases entre o inicio do bloco variavel e o inicio da regiao 5' a esquerda.", NULL },
		{ "tam5l", NULL, 0, G_OPTION_ARG_INT, &tam_regiao_5l, "Define o tamanho da regiao 5'.", NULL },
		{ "disablecuda", 'd', 0, G_OPTION_ARG_NONE, &disable_cuda, "Impede o processamento atraves da arquitetura CUDA.", NULL },
		{ "fromFile", 'f', 0, G_OPTION_ARG_STRING, &fromFile, "Carrega a configuracao de busca de um arquivo de texto.", NULL },
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

char* trocar_barra(char *s){
	const int len = strlen(s);
	char *n_s = (char*)malloc((len+1)*sizeof(char));
	int i;
	for(i=0;i<=len;i++){
		if(s[i] == '\\'){
			n_s[i] = '/';
		}else{
			n_s[i] = s[i];
		}
	}
	return n_s;
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
	FILE *f;
	lista_ligada *resultados;
	Params set;
	time_t t;
	char *tempo;

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

	// Inicializa
	// Seta nome padrao de saida
	time(&t);
	tempo = (char*)malloc(500*sizeof(char));
	strcpy(tempo,ctime(&t));	
	tempo[strlen(tempo)-1] = '\0';
	if(output_dir != NULL){
		if(output_dir[strlen(output_dir)-1] == '/')
			output_dir[strlen(output_dir)-1] = '\0';
	}else{
		output_dir = (char*)malloc(100*sizeof(char));
		printf("Output: %s\n",DEFAULT_OUTPUT_DIR);
		strcpy(output_dir,DEFAULT_OUTPUT_DIR);
		output_dir = trocar_barra(output_dir);
	}
	// Seta log
	prepareLog(output_dir,tempo,gui_run);
	printString("Diretório de saída: ",output_dir);		
	c = NULL;
	nome = NULL;

	if(just_process){
		if(!silent || gui_run)
			printf("Iniciando em modo de processamento...\n");
			f = fopen(argv[1],"r");
	}else{

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

		bibliotecas_validas = open_file(argv,argc,silent);
		if(bibliotecas_validas == 0){
			printf("Por favor, entre uma biblioteca válida.\n");
			exit(1);
		}
		seqs_validas = check_sequencias_validas(silent);

		//////////////////////////////////
		////////////////////////////////////////////////////////
		if(fromFile){
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
			nome = (char*)malloc((1000)*sizeof(char));
			fscanf(set,"%s",nome);

		}else{
			if(target_seq){
				strcpy(c,target_seq);
				if(target_name){
					nome = (char*)malloc((10000)*sizeof(char));
					strcpy(nome,target_name);
				}
			}else{
				if(!silent)
					printf("Entre a sequência: ");
				scanf("%s",c);
				if(!c){
					printf("Erro na leitura\n");
					exit(1);
				}
				if(!silent && !gui_run)
					printf("Entre uma identificação para essa busca: ");
				nome = (char*)malloc((1000)*sizeof(char));
				scanf("%s",nome);
			}
		}



		if(!check_seq(c,&b1_size,&b2_size,&bv_size)){
			printf("Sequência de busca inválida\n");
			exit(1);
		}  
		if(nome)
			printString("Identificação da busca: ",nome);
		if(c)
			printString("Sequência de busca: ",c);

		c_size = b1_size+b2_size+bv_size;


		// Seta database
		if(cutmode)  
			criar_db_manager(output_dir,tempo,bv_size);
		else
			criar_db_manager(output_dir,tempo,get_setup());

		//Guarda parametros
		set.verbose = verbose;
		set.silent = silent;
		set.debug = debug;
		set.cut_central = cutmode;
		set.gui_run = gui_run;
		set.dist_regiao_5l = dist_regiao_5l;
		set.tam_regiao_5l = tam_regiao_5l;
		
		if(disable_cuda){
			if(!silent || gui_run)
				printf("Forçando modo OpenMP.\n");
			printString("Forçando modo OpenMP.",NULL);
			aux(0,c,b1_size,b2_size,c_size,set); 
		}
		else{
			aux(is_cuda_available,c,b1_size,b2_size,c_size,set);
		}
		free(c);
	}

	//resultados = processar(bv_size,max_events,silent,gui_run);


	//if(!gui_run)
	//	imprimir(resultados,tempo,max_events,silent,gui_run);

	close_file();
	destroy_db_manager();

	if(!silent)
		printf("Algoritmo concluído.\n");

	return 0;
}
