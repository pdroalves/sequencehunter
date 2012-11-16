#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "ghashtable.h"
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "log.h"
#include "fila.h"

#define OMP_NTHREADS 4
#define THREAD_BUFFER_LOADER 0
#define THREAD_SEARCH 1
#define THREAD_RESULTS 2
#define THREAD_CLEANER 3

omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
__constant__ char *d_buffer[buffer_size];
omp_lock_t DtH_copy_lock;
gboolean THREAD_DONE[OMP_NTHREADS];

char* convertResultToChar(int n){
	char *tipo;
	tipo = (char*)malloc(10*sizeof(char));
	
	switch(n){
		case SENSO:
			strcpy(tipo,"SENSO");
			return tipo;
		break;
		case ANTISENSO:
			strcpy(tipo,"ANTISENSO");
			return tipo;
		break;
	}
}
int convertResultToInt(char *tipo){
	if(strcmp("SENSO",tipo) == 0) return SENSO;
	else return ANTISENSO;
}

void load_buffer_CUDA(char **h_seqs,char **d_seqs,int seq_size,int *load,cudaStream_t stream){
	int i;
	int loaded;
	loaded = 0;
	
	if(*load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
		loaded = fill_buffer_CUDA(h_seqs,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
		if(loaded != -1){
			print_seqs_carregadas(loaded);
			//Copia sequencias para GPU
			for(i=0;i<loaded;i++)
				cudaHostGetDevicePointer(&d_seqs[i],h_seqs[i],0);
				//cudaMemcpyAsync(d_seqs[i],h_seqs[i],(seq_size+1)*sizeof(char),cudaMemcpyHostToDevice,stream);
			cudaMemcpyAsync(data,d_seqs,loaded*sizeof(char*),cudaMemcpyHostToDevice,stream);	
		}		
		*load = loaded;	
	}
	
	return;
}



void buffer_manager(int *buffer_load,char **h_data,char **d_data,int n,cudaStream_t stream1){
				//////////////////////////////////////////
				// Carrega o buffer //////////////////////
				//////////////////////////////////////////
				THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
					while(*buffer_load != GATHERING_DONE){//Looping até o final do buffer
					//printf("%d.\n",buffer.load);
					if(*buffer_load == 0){
						load_buffer_CUDA(h_data,d_data,n,buffer_load,stream1);
					}	
				}
				THREAD_DONE[THREAD_BUFFER_LOADER] = TRUE;
				return ;
				//////////////////////////////////////////
				//////////////////////////////////////////
				//////////////////////////////////////////	
}




void search_manager(int *buffer_load,int *processadas,Fila *tipo_founded,Fila *founded,int bloco1,int bloco2,int blocoV,cudaStream_t stream1,cudaStream_t stream2,int **d_matrix_senso,int **d_matrix_antisenso){
				THREAD_DONE[THREAD_SEARCH] = FALSE;
				int i;
				int blocos;
				int *h_resultados;
				char **h_founded;
				char **d_founded;
				char **dp_founded;	
				int *d_resultados;
				int loaded;
				blocos = blocoV + bloco1 + bloco2 - 1;
				cudaMalloc((void**)&d_resultados,buffer_size*sizeof(int));
				cudaHostAlloc((void**)&d_founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
				cudaMalloc((void**)&dp_founded,buffer_size*sizeof(char*));
				for(i=0;i<buffer_size;i++)
					cudaMalloc((void**)&d_founded[i],(blocoV+1)*sizeof(char));
				cudaMemcpyAsync(dp_founded,d_founded,buffer_size*sizeof(char*),cudaMemcpyHostToDevice,stream2);
				cudaHostAlloc((void**)&h_founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
				for(i=0;i<buffer_size;i++)
					cudaHostAlloc((void**)&h_founded[i],(blocoV+1)*sizeof(char),cudaHostAllocDefault);
				cudaHostAlloc((void**)&h_resultados,buffer_size*sizeof(int),cudaHostAllocDefault);	
	
				while( *buffer_load == 0){
				}//Aguarda para que o buffer seja enchido pela primeira vez
				
				while( *buffer_load != GATHERING_DONE){
				//Realiza loop enquanto existirem sequências para encher o buffer
						k_busca(*buffer_load,bloco1,bloco2,blocos,data,d_resultados,dp_founded,d_matrix_senso,d_matrix_antisenso,stream1);//Kernel de busca
						omp_set_lock(&DtH_copy_lock);
						// Inicia processamento dos resultados
						cudaStreamSynchronize(stream1);
						//printf("%d\n",p);
						loaded = *buffer_load;
						*buffer_load = 0;	
						*processadas += loaded;
						cudaMemcpy(h_resultados,d_resultados,buffer_size*sizeof(int),cudaMemcpyDeviceToHost);
						checkCudaError();
						for(i=0;i<loaded;i++)
							if(h_resultados[i] != 0){
								cudaMemcpyAsync(h_founded[i],d_founded[i],(blocoV+1)*sizeof(char),cudaMemcpyDeviceToHost,stream2);
								checkCudaError();
							}
						cudaStreamSynchronize(stream2);
						for(i=0;i<loaded;i++)
							if(h_resultados[i] != 0){
								enfileirar(founded,h_founded[i]);
								enfileirar(tipo_founded,convertResultToChar(h_resultados[i]));
							}
						checkCudaError();
						if(verbose && !silent)
							printf("Sequencias processadas: %d\n",*processadas);
						omp_unset_lock(&DtH_copy_lock);
						//print_fila(founded);
						while(*buffer_load==0){}
				}//Aguarda para que o buffer seja enchido pela primeira vez
				cudaFreeHost(d_resultados);
				THREAD_DONE[THREAD_SEARCH] = TRUE;
				return;
}

void results_manager(int *buffer_load,int processadas,Fila* tipo_founded,Fila *founded,Fila *f_sensos,Fila *f_antisensos){
	//////////////////////////////////////////
				// Realiza o processamento das iteracoes//
				//////////////////////////////////////////
				THREAD_DONE[THREAD_RESULTS] = FALSE;
				int i;
				int resultado;
				char *tmp;
				
				
				while(*buffer_load != GATHERING_DONE || !THREAD_DONE[THREAD_SEARCH]){
						omp_set_lock(&DtH_copy_lock);
						// Essa parte pode ser feita em paralelo
						i=0;
						while(tamanho_da_fila(founded) > 0){//Copia sequências senso e antisenso encontradas
							resultado = convertResultToInt(desenfileirar(tipo_founded)); 
							tmp = desenfileirar(founded);
							switch(resultado){
								case SENSO:
									if(verbose && !silent)
										printf("S: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_sensos));
									enfileirar(f_sensos,tmp);
								break;
								case ANTISENSO:
									if(verbose && !silent)
										printf("N: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_antisensos));
									enfileirar(f_antisensos,get_antisenso(tmp));
								break;
							}
							i++;
						}
						omp_unset_lock(&DtH_copy_lock);
											
						/*if(seqsToProcess != 0 || buffer_load !=0)
						{
							//printf("Erro! Buffer não foi totalmente esvaziado.\n");
							seqsToProcess = 0;
							buffer_load = 0;
						}*/
										
						}
				THREAD_DONE[THREAD_RESULTS] = TRUE;
				return ;
}

void memory_cleaner_manager(GHashTable* hash_table,int *buffer_load,Fila *f_sensos,Fila *f_antisensos){
					//////////////////////////////////////////
					// Libera memoria ////////////////////////
					//////////////////////////////////////////
					THREAD_DONE[THREAD_CLEANER] = FALSE;
					  int retorno;
					  			  
					  while( *buffer_load == 0){
						}//Aguarda para que o buffer seja enchido pela primeira vez
						

					  while(*buffer_load != GATHERING_DONE || !THREAD_DONE[THREAD_RESULTS]){
						if(tamanho_da_fila(f_sensos) > 0){
							retorno = adicionar_ht(hash_table,desenfileirar(f_sensos),criar_value(0,1,0,0));
						}
						if(tamanho_da_fila(f_antisensos) > 0){
							retorno = adicionar_ht(hash_table,desenfileirar(f_antisensos),criar_value(0,0,1,0));
						}
					  }
						
						while(tamanho_da_fila(f_sensos) > 0){
							retorno = adicionar_ht(hash_table,desenfileirar(f_sensos),criar_value(0,1,0,0));
						}
						while(tamanho_da_fila(f_antisensos) > 0){
							retorno = adicionar_ht(hash_table,desenfileirar(f_antisensos),criar_value(0,0,1,0));
						}
					
					if(g_hash_table_size(hash_table) == 0)
						printf("Ta vazio!\n");
		
					THREAD_DONE[THREAD_CLEANER] = TRUE;
					return;
					//////////////////////////////////////////
					//////////////////////////////////////////
					//////////////////////////////////////////
}


GHashTable* cudaIteracoes(int bloco1,int bloco2,int blocos,int n,int **d_matrix_senso,int **d_matrix_antisenso){
	
	
	Buffer buffer;
	int blocoV = blocos - bloco1 - bloco2+1;
	int i;
	int processadas;
	int buffer_load;
	Fila *f_sensos;
	Fila *f_antisensos;
	Fila *founded;
	Fila *tipo_founded;
	char **h_data;
	char **d_data;		
	cudaStream_t stream1;
	cudaStream_t stream2;
	GHashTable* hash_table;

	//Inicializa buffer
	cudaStreamCreate(&stream1);
	cudaStreamCreate(&stream2);
	hash_table = criar_ghash_table();		
	cudaHostAlloc((void**)&founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
	for(i=0;i<buffer_size;i++)
		cudaHostAlloc((void**)&founded[i],(blocoV+1)*sizeof(char),cudaHostAllocDefault);
	
	buffer_load = 0;
	processadas=0;
	f_sensos = criar_fila("Sensos");
	f_antisensos = criar_fila("Antisensos");
	founded = criar_fila("Founded");
	tipo_founded = criar_fila("Tipo Founded");
	start_fila_lock();
	omp_init_lock(&DtH_copy_lock);
	cudaMalloc((void**)&data,buffer_size*sizeof(char*));
	cudaHostAlloc((void**)&h_data,buffer_size*sizeof(char*),cudaHostAllocMapped | cudaHostAllocWriteCombined);
	for(i=0;i<buffer_size;i++)
		cudaHostAlloc((void**)&h_data[i],(n+1)*sizeof(char),cudaHostAllocDefault);
	cudaHostAlloc((void**)&d_data,buffer_size*sizeof(char*),cudaHostAllocDefault);
	for(i=0;i<buffer_size;i++)
			cudaMalloc((void**)&d_data[i],(n+1)*sizeof(char));
		
	#pragma omp parallel num_threads(OMP_NTHREADS) shared(hash_table) shared(buffer) shared(f_sensos) shared(f_antisensos) shared(buffer_load) shared(founded) shared(stream1) shared(stream2) shared(tipo_founded)
	{	
		
		#pragma omp sections
		{
			#pragma omp section
			{
				buffer_manager(&buffer_load,h_data,d_data,n,stream1);
			}
			#pragma omp section
			{
				search_manager(&buffer_load,&processadas,tipo_founded,founded,bloco1,bloco2,blocoV,stream1,stream2,d_matrix_senso,d_matrix_antisenso);
			}		
			#pragma omp section
			{
				results_manager(&buffer_load,processadas,tipo_founded,founded,f_sensos,f_antisensos);
			}
			#pragma omp section
			{
				memory_cleaner_manager(hash_table,&buffer_load,f_sensos,f_antisensos);
			}
		}
	}
	//printf("Iterações executadas: %d.\n",iter);
	//free(tmp);
	cudaDeviceReset();
	omp_destroy_lock(&DtH_copy_lock);
	//cudaStreamDestroy(stream1);
	//cudaStreamDestroy(stream2);
	/*for(i=0;i<buffer_size;i++){
		cudaFreeHost(founded[i]);
		cudaFreeHost(h_data[i]);
	}	
	cudaFreeHost(founded);
	cudaFreeHost(h_data);
	cudaFreeHost(d_data);*/
	cudaFree(data);
	return hash_table;
}





void setup_for_cuda(char *seq,int **d_matrix_senso,int **d_matrix_antisenso){
	// Recebe um vetor de caracteres com o padrão a ser procurado
	// As matrizes não precisam estar alocadas
	char *senso;
	char *antisenso;
	int **h_matrix_senso;
	int **h_matrix_antisenso;
	int size = strlen(seq);
	int i;
	
	// Aloca memória na CPU
	cudaMalloc((void**)&d_matrix_senso,size*sizeof(int*));
	cudaMalloc((void**)&d_matrix_antisenso,size*sizeof(int*));
	
	h_matrix_senso = (int**)malloc(size*sizeof(int*));
	h_matrix_antisenso = (int**)malloc(size*sizeof(int*));
	
	// Aloca memória na GPU
	for(i = 0; i < size ; i++){
		cudaMalloc((void**)&h_matrix_senso[i],N_COL*sizeof(int));
		cudaMalloc((void**)&h_matrix_antisenso[i],N_COL*sizeof(int));
	}
	cudaMalloc((void**)&senso,(size+1)*sizeof(char));
	cudaMalloc((void**)&antisenso,(size+1)*sizeof(char));
	
	// Copia dados
    cudaMemcpy(d_matrix_senso,h_matrix_senso,size*sizeof(int*),cudaMemcpyHostToDevice);
    cudaMemcpy(d_matrix_antisenso,h_matrix_antisenso,size*sizeof(int*),cudaMemcpyHostToDevice);
      
    cudaMemcpy(senso,seq,(size+1)*sizeof(char),cudaMemcpyHostToDevice);
    cudaMemcpy(antisenso,(const void*)get_antisenso(seq),(size+1)*sizeof(char),cudaMemcpyHostToDevice);
    
    //Configura grafos direto na memória da GPU
	set_grafo_helper(senso,antisenso,d_matrix_senso,d_matrix_antisenso);
	printString("Grafo de busca contigurado.",NULL);
	cudaFree(senso);
	cudaFree(antisenso);
	return;
}

GHashTable* auxCUDA(char *c,const int bloco1,const int bloco2,const int blocos,gboolean verb,gboolean sil){
	printf("CUDA Mode.\n");
	int n;//Elementos por sequência
	cudaEvent_t start;
	cudaEvent_t stop;
	GHashTable* hash_table;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	float tempo;
	int **d_matrix_senso;
	int **d_matrix_antisenso;
	
	verbose = verb;
	silent = sil;
	
	get_setup(&n);
    
	//Inicializa
	setup_for_cuda(c,d_matrix_senso,d_matrix_antisenso);
	
	printString("Dados inicializados.\n",NULL);
	printSet(n);
	printString("Iniciando iterações:\n",NULL);
	
   // cudaEventRecord(start,0);
    hash_table = cudaIteracoes(bloco1,bloco2,blocos,n,d_matrix_senso,d_matrix_antisenso);
   // cudaEventRecord(stop,0);
   // cudaEventSynchronize(stop);
   // cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	cudaThreadExit();
	return hash_table;

}
