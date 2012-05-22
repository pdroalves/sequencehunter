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
	#include "pilha.h"
	#include "processing_data.h"

	#define SEQ_BUSCA_TAM 1000

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
	
	// General GPU Device CUDA Initialization
int gpuDeviceInit(int devID)
{
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0)
    {
        fprintf(stderr, "gpuDeviceInit() CUDA error: no devices supporting CUDA.\n");
        exit(-1);
    }

    if (devID < 0)
       devID = 0;
        
    if (devID > deviceCount-1)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, ">> %d CUDA capable GPU device(s) detected. <<\n", deviceCount);
        fprintf(stderr, ">> gpuDeviceInit (-device=%d) is not a valid GPU device. <<\n", devID);
        fprintf(stderr, "\n");
        return -devID;
    }

    cudaDeviceProp deviceProp;
   cudaGetDeviceProperties(&deviceProp, devID);

    if (deviceProp.major < 1)
    {
        fprintf(stderr, "gpuDeviceInit(): GPU device does not support CUDA.\n");
        exit(-1);                                                  
    }
    
    cudaSetDevice(devID);
    printf("gpuDeviceInit() CUDA Device [%d]: \"%s\n", devID, deviceProp.name);

    return devID;
}

inline int _ConvertSMVer2Cores(int major, int minor)
{
	// Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
	typedef struct {
		int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] = 
	{ { 0x10,  8 },
	  { 0x11,  8 },
	  { 0x12,  8 },
	  { 0x13,  8 },
	  { 0x20, 32 },
	  { 0x21, 48 },
	  {   -1, -1 } 
	};

	int index = 0;
	while (nGpuArchCoresPerSM[index].SM != -1) {
		if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor) ) {
			return nGpuArchCoresPerSM[index].Cores;
		}
		index++;
	}
	printf("MapSMtoCores undefined SMversion %d.%d!\n", major, minor);
	return -1;
}

// This function returns the best GPU (with maximum GFLOPS)
int gpuGetMaxGflopsDeviceId()
{
    int current_device     = 0, sm_per_multiproc  = 0;
    int max_compute_perf   = 0, max_perf_device   = 0;
    int device_count       = 0, best_SM_arch      = 0;
    cudaDeviceProp deviceProp;
    cudaGetDeviceCount( &device_count );
    
    // Find the best major SM Architecture GPU device
    while (current_device < device_count)
    {
        cudaGetDeviceProperties( &deviceProp, current_device );
        if (deviceProp.major > 0 && deviceProp.major < 9999)
        {
            best_SM_arch = MAX(best_SM_arch, deviceProp.major);
        }
        current_device++;
    }

    // Find the best CUDA capable GPU device
    current_device = 0;
    while( current_device < device_count )
    {
        cudaGetDeviceProperties( &deviceProp, current_device );
        if (deviceProp.major == 9999 && deviceProp.minor == 9999)
        {
            sm_per_multiproc = 1;
        }
        else
        {
            sm_per_multiproc = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
        }
        
        int compute_perf  = deviceProp.multiProcessorCount * sm_per_multiproc * deviceProp.clockRate;
        
    if( compute_perf  > max_compute_perf )
    {
            // If we find GPU with SM major > 2, search only these
            if ( best_SM_arch > 2 )
            {
                // If our device==dest_SM_arch, choose this, or else pass
                if (deviceProp.major == best_SM_arch)
                {
                    max_compute_perf  = compute_perf;
                    max_perf_device   = current_device;
                 }
            }
            else
            {
                max_compute_perf  = compute_perf;
                max_perf_device   = current_device;
             }
        }
        ++current_device;
    }
    return max_perf_device;
}


// Initialization code to find the best CUDA Device
int findCudaDevice()
{
    cudaDeviceProp deviceProp;
    int devID = 0;
    
    // Escolhe o device com maior taxa de Gflops/s
    devID = gpuGetMaxGflopsDeviceId();
    cudaSetDevice( devID );
    cudaGetDeviceProperties(&deviceProp, devID);
    printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID, deviceProp.name, deviceProp.major, deviceProp.minor);
    
    return devID;
}
// end of CUDA Helper Functions

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
	  
	  gpuDeviceInit(findCudaDevice());
	  
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
	  err = open_file(argv,argc);
	  if(err == 0){
		  printf("Arquivo inválido: $s.\n",argv[1]);
		  exit(1);
	  }
	////////////////////////////////////////////////////////
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
	  
	  aux(1,c,b1_size,b2_size,c_size,&p_sensos,&p_antisensos);
	  processar(&p_sensos,&p_antisensos);
	  
	 close_file();
	 free(c);
	destroy(&p_sensos);
	destroy(&p_antisensos);
	return 0;
	}
