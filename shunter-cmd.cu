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
#include "estruturas.h"
#include <cuda.h>
#include <cuda_runtime_api.h>
//#include "operacoes.h"
//#include "linkedlist.h"
#include "aux.h"
#include "log.h"
#include <glib.h>
#include "load_data.h"

//###############
//Parametros de entrada
static gint tiros = 1;
static gdouble inttiros_ = 100;
static gboolean silent = FALSE;
static gboolean verbose = FALSE;
static gint placa = FALSE;
static int CUDA;

static GOptionEntry entries[] = 
  {
    //O comando "rápido" suporta 1 caracter na chamada. Se for usado mais que isso, pode dar pau
    //Entrada de posicoes
    { "tiros", 't', 0, G_OPTION_ARG_INT, &tiros, "Quantidade de Tiros - Default: 1", NULL },
    { "escolherplaca", 'e', 0, G_OPTION_ARG_NONE, &placa, "Permite que o usuário escolha qual placa de vídeo deve ser usada", NULL },
    { "intervalodetiros", 'i', 0, G_OPTION_ARG_DOUBLE, &inttiros_, "Distancia entre cada tiro - Default: 100", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
    { "silent", 's', 0, G_OPTION_ARG_NONE, &silent, "Execução silenciosa", NULL },
    { NULL }
  };
//#######################

void getDevice(int deviceCount,cudaDeviceProp deviceProp){

  int i;
  int device;
  cudaError_t erro;

     printf("Dispositivos encontrados:\n");
	  for(i=0;i<deviceCount;i++)
	    {
	      erro = cudaGetDeviceProperties(&deviceProp,i);
	      g_assert(erro == cudaSuccess);
	      printf("%d) %s\n",i,deviceProp.name);
	    }

	  printf("\n Usar dispositivo:");
	  scanf("%d",&device);
		
	  while(device > deviceCount)
	    {
	      printf("Dispositivo invalido\n");
	      scanf("%d",&device);
	    }
	
    //Libera o dispositivo em uso
    erro = cudaThreadExit();
    g_assert(erro == cudaSuccess);
	
    //Configura qual dispositivo deve ser usado
    erro = cudaSetDevice(device);
    g_assert(erro == cudaSuccess);
		
    //O dispositivo foi escolhido
    erro = cudaGetDeviceProperties(&deviceProp,device);
    g_assert(erro == cudaSuccess);
	
    if(verbose == TRUE) printf("Dispositivo configurado para uso: %s\n\n",deviceProp.name); 
 
}

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
  int i;
  int deviceCount;
  int device;
  int err;
  cudaDeviceProp tmp;
  cudaDeviceProp deviceProp;
  cudaError_t erro;
  
  //Inicializa
  prepareLog();
  cudaGetDeviceCount(&deviceCount);
  
    // Função que verifica se existe um dispositivo que suporte CUDA.
	if(deviceCount == 0){
	  printf("Nao existe um dispositivo instalado na maquina que suporte CUDA\n");
	  CUDA = 0;//Sem suporte a CUDA
	}else{
		CUDA = 1;//Suporte a CUDA
	}
		
	if(placa == false){ 	
	   	for(i = 1; i < deviceCount;i++) {
			erro = cudaGetDeviceProperties(&tmp,i-1);
		   	g_assert(erro == cudaSuccess);
		   	erro = cudaGetDeviceProperties(&deviceProp,i);	     
	    	g_assert(erro == cudaSuccess);
	     	if(tmp.multiProcessorCount > deviceProp.multiProcessorCount) device = i-1;
	   		else device = i;
		  	}
	  	}else
	    	getDevice(device,deviceProp);
	    	
	/*    cudaGetDeviceProperties(&deviceProp,device);
	    if(!deviceProp.deviceOverlap){
	    	printf("Placa gráfica não suporta overlaps. Não haverá ganho no uso de steams\n");
	    }*/
  	

  
  printf("Qual o tamanho da sequência?\n");
  scanf("%d",&c_size);
  
  c = (char*)malloc(c_size*sizeof(char));
  
  printf("Entre a sequência: ");
  scanf("%s",c);
  
  printString("Sequência de busca: ",c);
  
  err = open_file("sequências.dat");
  if(err == 0){
	  printf("Erro de arquivo.\n");
	  exit(1);
  }
  
  aux(CUDA);
  
  close_file();
return 0;
}
