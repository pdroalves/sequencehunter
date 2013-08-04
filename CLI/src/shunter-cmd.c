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
#include "Headers/version.h"
#include "Headers/database_manager.h"
#include "Headers/database.h"
#include "Headers/socket.h"

#define VERSION "v1.1.0"

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
gboolean disable_cpu = FALSE;
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
gboolean fixdb = FALSE;
//###############
static GOptionEntry entries[] =
  {
    //O comando "rapido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
    //Entrada de posicoes
    { "target", 'a', 0, G_OPTION_ARG_STRING, &target_seq, "Defines target sequence.", NULL },
    { "output", 'o', 0, G_OPTION_ARG_STRING, &output_dir, "Defines output folder (home folder by default).", NULL },
    { "dist5l", NULL, 0, G_OPTION_ARG_INT, &dist_regiao_5l, "Define a quantidade de bases entre o inicio do bloco variavel e o inicio da regiao 5' a esquerda.", NULL },
    { "tam5l", NULL, 0, G_OPTION_ARG_INT, &tam_regiao_5l, "Define o tamanho da regiao 5'.", NULL },
    { "disablecuda", 'd', 0, G_OPTION_ARG_NONE, &disable_cuda, "Force processing by CPU.", NULL },
    { "disablecpu", 'e', 0, G_OPTION_ARG_NONE, &disable_cpu, "Force processing by GPU.", NULL },
    { "fromFile", 'f', 0, G_OPTION_ARG_STRING, &fromFile, "Loads hunt settings from a text file.", NULL },
    { "cutseqs", 't', 0, G_OPTION_ARG_NONE, &cutmode, "Stores only variable block.", NULL },
    { "silent", 's', 0, G_OPTION_ARG_NONE, &silent, "Silent execution.", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose.", NULL },
    { "build", 'b', 0, G_OPTION_ARG_NONE, &check_build, "Gets build number.", NULL },
    { "fixdb", 'x', 0, G_OPTION_ARG_NONE, &fixdb, "Try to fix a database file.", NULL },
    { "debug", NULL, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &debug, NULL, NULL },		
    { "gui", NULL, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &gui_run, NULL, NULL },	
    { NULL }
  };

int get_build(){

  return build;
}

void trocar_barra(char *s){
  const int len = strlen(s);
  int i;
  for(i=0;i<=len;i++){
    if(s[i] == '\\'){
      s[i] = '/';
    }
  }
  return ;
}

void remove_barra_final(char *s){
  const int len = strlen(s);
  if(s[len-1] == '/')
	  s[len-1] = '\0';
  return;
}

void processar_output_path(char *s){
  // Inverte barras, se necessario
    trocar_barra(output_dir);
  // Remove barra final, se existir
    remove_barra_final(output_dir);
return;
}

//####################
int main (int argc,char *argv[]) {

  GError *error = NULL;
  GOptionContext *context;
  char *c;
  int c_size;
  int b1_size;
  int b2_size;
  int bv_size;
  int is_cuda_available;
  int bibliotecas_validas;
  Params set;
  time_t t;
  char *tempo;
  Socket *gui_socket;

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

    if(gui_run){
    // Handshake com a GUI
    gui_socket = (Socket*)malloc(sizeof(Socket));
    configure_socket(gui_socket);
  }


  if(check_build){
    printf("%s.%d\n",VERSION,get_build());
    return 0;
  }

  if(fixdb){
    printf("Trying to fix %s...\n",argv[1]);
    fix_database(argv[1]);
    if(gui_run){
      send_msg_to_socket(gui_socket,SKT_MSG_ABORT);
      destroy_socket(gui_socket);
    }
    exit(1);
  }
  if(!silent)
    printf("Starting Sequence Hunter...\nBuild: %d\n",get_build());
  if(verbose && !silent)
    printf("Verbose Mode\n");



  // Inicializa
  // Seta nome padrao de saida
  time(&t);
  tempo = (char*)malloc(500*sizeof(char));
  strcpy(tempo,ctime(&t));	
  tempo[strlen(tempo)-1] = '\0';
  if(output_dir != NULL){
    processar_output_path(output_dir);
  }else{	
    output_dir = (char*)malloc(1000*sizeof(char));
    strcpy(output_dir,DEFAULT_OUTPUT_DIR);
    processar_output_path(output_dir);
  }
    if(!silent)
      printf("Output: %s\n",output_dir);

  // Seta log
  prepareLog(output_dir,tempo,gui_run);
  printString("Output Folder: ",output_dir);		
  c = NULL;

  /*if(just_process){
    if(!silent || gui_run)
    printf("Starting on processing mode...\n");
    f = fopen(argv[1],"r");
    }else{*/

  c = (char*)malloc((SEQ_BUSCA_TAM+1)*sizeof(char));

  if(c == NULL){
    printf("Memory error.\n");
    if(gui_run){
      send_msg_to_socket(gui_socket,SKT_MSG_ABORT);
      destroy_socket(gui_socket);
    }
    exit(1);
  }

  
  ////////////////////////////////////////////////////////
  ////////////////// Abre arquivos de bibliotecas/////////
  ////////////////////////////////////////////////////////
  if(argc == 1){
    printf("Please, enter a valid fastm library.\n");
    if(gui_run){
      send_msg_to_socket(gui_socket,SKT_MSG_ABORT);
      destroy_socket(gui_socket);
    }
    exit(1);
  }

  bibliotecas_validas = open_file(argv,argc,silent);
  if(bibliotecas_validas == 0){
    printf("Please, enter a valid fastm library.\n");
    if(gui_run){
      send_msg_to_socket(gui_socket,SKT_MSG_ABORT);
      destroy_socket(gui_socket);
    }
    exit(1);
  }
 
  //////////////////////////////////
  ////////////////////////////////////////////////////////
  if(fromFile){
    FILE *set;
    set = fopen(fromFile,"r");
    if(set == NULL){
      printf("%s not found.\n",fromFile);
      exit(1);
    }
    if(!silent)
      printf("Settings loaded from %s\n",fromFile);
    fscanf(set,"%s",c);
    if(c == NULL){
      printf("Read error.\n");
      exit(1);
    }
   
  }else{
    if(target_seq){
      strcpy(c,target_seq);
     
    }else{
      if(!silent)
	printf("Target sequence: ");
      scanf("%s",c);
      if(!c){
	printf("Read error.\n");
	exit(1);
      }
    }
  }

  if(!check_seq(c,&b1_size,&b2_size,&bv_size)){
    printf("Invalid target sequence.\n");
    exit(1);
  }  
  if(c)
    printString("Target sequence: ",c);

  c_size = b1_size+b2_size+bv_size;

  // Cria objeto para se comunicar com a GUI

  // Seta database
  if(cutmode)  
    criar_db_manager(output_dir,tempo,bv_size,silent);
  else
    criar_db_manager(output_dir,tempo,get_setup(),silent);

  //Guarda parametros
  set.verbose = verbose;
  set.silent = silent;
  set.debug = debug;
  set.cut_central = cutmode;
  set.gui_run = gui_run;
  set.dist_regiao_5l = dist_regiao_5l;
  set.tam_regiao_5l = tam_regiao_5l;
		

  if(disable_cuda){
    // Forca execucao pela CPU
    if(!silent || gui_run)
      aux(0,c,b1_size,b2_size,c_size,set,gui_socket); 
  }else{
    if(disable_cpu){
      // Forca execucao pela GPU
      aux(1,c,b1_size,b2_size,c_size,set,gui_socket);
    }else{
      // SH determina a melhor forma de execucao
      is_cuda_available  = check_gpu_mode();
      aux(is_cuda_available,c,b1_size,b2_size,c_size,set,gui_socket);
    }
  }
  free(c);
  //}

  //resultados = processar(bv_size,max_events,silent,gui_run);


  //if(!gui_run)
  //	imprimir(resultados,tempo,max_events,silent,gui_run);

  close_file();
  destroy_db_manager();
  if(gui_run){
    destroy_socket(gui_socket);
    if(gui_socket != NULL)
      free(gui_socket);
  }

  if(!silent)
    printf("Concluded.\n");
  if(output_dir != NULL)
    free(output_dir);

  return 0;
}
