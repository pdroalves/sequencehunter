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

#define OMP_NTHREADS 3
#define THREAD_BUFFER_LOADER 0
#define THREAD_SEARCH 1
#define THREAD_RESULTS 2
#define THREAD_CLEANER 3

omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
__constant__ short int **d_matrix_senso;
__constant__ short int **d_matrix_antisenso;
__constant__ char **data;
omp_lock_t DtH_copy_lock;
omp_lock_t MC_copy_lock;
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



void buffer_manager(int *buffer_load,
						int n,
						char **h_data,
						char **d_data,
						cudaStream_t stream1){
							
				//////////////////////////////////////////
				// Carrega o buffer //////////////////////
				//////////////////////////////////////////
				THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
				int i;
										
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




void search_manager(int *buffer_load,
							int *processadas,
							Fila *tipo_founded,
							Fila *founded,
							const int seqSize_an,
							const int seqSize_bu,
							int bloco1,
							int bloco2,
							int blocoV,
							cudaStream_t stream1,
							cudaStream_t stream2,
							short int **d_matrix_senso,
							short int **d_matrix_antisenso,
							char **h_data,
							char **d_data,
							Fila *f_sensos,
							Fila *f_antisensos){
								
				THREAD_DONE[THREAD_SEARCH] = FALSE;
				int i;
				short int *h_resultados;
				char **h_founded;
				short int *h_gap;
				short int *d_gap;
				short int *d_resultados;
				int loaded;
				int hold;
				int p;
				float iteration_time;
				
				cudaEvent_t startK,stopK;
				cudaEvent_t start,stop;
				char **local_data;
				cudaEvent_t startV,stopV;
				float elapsedTimeK,elapsedTime;
				FILE *busca,*retorno;
				
				busca = fopen("cuda_busca.dat","w");
				retorno = fopen("cuda_retorno.dat","w");
				
				cudaEventCreate(&start);
				cudaEventCreate(&stop);
				cudaEventCreate(&startK);
				cudaEventCreate(&stopK);
				
				h_gap = (short int*)malloc(buffer_size*sizeof(short int));
				cudaMalloc((void**)&d_gap,buffer_size*sizeof(short int));
				cudaMalloc((void**)&d_resultados,buffer_size*sizeof(short int));
				cudaHostAlloc((void**)&h_founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
				for(i=0;i<buffer_size;i++)
					cudaHostAlloc((void**)&h_founded[i],(blocoV+1)*sizeof(char),cudaHostAllocDefault);
				h_resultados = (short int*)malloc(buffer_size*sizeof(short int));
				local_data = (char**)malloc(buffer_size*sizeof(char*));
				for(i=0;i<buffer_size;i++)
					local_data[i] = (char*)malloc((seqSize_an+1)*sizeof(char));
				
				iteration_time = 0;
				
				while( *buffer_load == 0){
				}//Aguarda para que o buffer seja enchido pela primeira vez
				
						cudaEventRecord(start,0);
				while( *buffer_load != GATHERING_DONE){
				//Realiza loop enquanto existirem sequências para encher o buffer
						cudaEventRecord(stop,0);
						cudaEventSynchronize(stop);
						cudaEventElapsedTime(&elapsedTime,start,stop);
						printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
						iteration_time += elapsedTime;
						//fprintf(retorno,"%f\n",elapsedTime);
						cudaEventRecord(startK,0);
						
						loaded = *buffer_load;
						k_busca(*buffer_load,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,data,d_resultados,d_gap,d_matrix_senso,d_matrix_antisenso,stream1);//Kernel de busca
						cudaEventRecord(stopK,0);						
						cudaEventSynchronize(stopK);
						cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
						printf("Execucao da busca em %.2f ms\n",elapsedTimeK);
						iteration_time += elapsedTimeK;
						//fprintf(busca,"%f\n",elapsedTimeK);
						cudaEventRecord(start,0);
						
						// Inicia processamento dos resultados
						cudaStreamSynchronize(stream1);
						*processadas += loaded;
							
						cudaMemcpyAsync(h_resultados,d_resultados,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost,stream2);
						cudaMemcpyAsync(h_gap,d_gap,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost,stream2);
						cudaStreamSynchronize(stream2);
						for(i=0;i<buffer_size;i++){
							if(h_resultados[i] != 0)
								strcpy(local_data[i],h_data[i]);
						}
						*buffer_load = 0;	
						for(i=0;i<loaded;i++)
							if(h_resultados[i] != 0){
								if(h_resultados[i] == SENSO) hold = bloco1 + h_gap[i] -1;
								else hold = bloco2 + h_gap[i] - 1;
								h_founded[i] = local_data[i]+hold;
								h_founded[i][blocoV]= '\0';
								/*omp_set_lock(&DtH_copy_lock);
								//printf("Sequencia: %s - tipo: %3d\n",h_founded[i],h_resultados[i]);
								enfileirar(founded,h_founded[i]);
								enfileirar(tipo_founded,convertResultToChar(h_resultados[i]));
								omp_unset_lock(&DtH_copy_lock);*/
								switch(h_resultados[i]){
									case SENSO:
										//if(verbose && !silent)
										//	printf("S: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_sensos));
										omp_set_lock(&MC_copy_lock);
										enfileirar(f_sensos,h_founded[i]);
										omp_unset_lock(&MC_copy_lock);
									break;
									case ANTISENSO:
										//if(verbose && !silent)
										//	printf("N: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_antisensos));
										omp_set_lock(&MC_copy_lock);
										enfileirar(f_antisensos,get_antisenso(h_founded[i]));
										omp_unset_lock(&MC_copy_lock);
									break;
								}
							}
						checkCudaError();
						if(verbose && !silent)
							printf("Sequencias analisadas: %d\n",*processadas);
						//print_fila(founded);
						while(*buffer_load==0){}
				}
				
				printf("Busca realizada em %.2f ms.\n",iteration_time);
				
				for(i=0;i<buffer_size;i++)
					free(local_data[i]);
				free(local_data);
				cudaFree(d_resultados);
				cudaFree(d_gap);
				for(i=0;i<buffer_size;i++)
					cudaFreeHost(h_founded[i]);
				cudaFreeHost(h_founded);
				free(h_resultados);
				free(h_gap);
				cudaEventDestroy(start);
				cudaEventDestroy(stop);
				cudaEventDestroy(startK);
				cudaEventDestroy(stopK);
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
						// Essa parte pode ser feita em paralelo
						i=0;
						while(tamanho_da_fila(founded) > 0){//Copia sequências senso e antisenso encontradas
							omp_set_lock(&DtH_copy_lock);
							resultado = convertResultToInt(desenfileirar(tipo_founded)); 
							tmp = desenfileirar(founded);
							omp_unset_lock(&DtH_copy_lock);
							switch(resultado){
								case SENSO:
									//if(verbose && !silent)
									//	printf("S: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_sensos));
									omp_set_lock(&MC_copy_lock);
									enfileirar(f_sensos,tmp);
									omp_unset_lock(&MC_copy_lock);
								break;
								case ANTISENSO:
									//if(verbose && !silent)
									//	printf("N: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_antisensos));
									omp_set_lock(&MC_copy_lock);
									enfileirar(f_antisensos,get_antisenso(tmp));
									omp_unset_lock(&MC_copy_lock);
								break;
							}
							i++;
						}
											
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
					  char *hold;
					  			  
					  while( *buffer_load == 0){
						}//Aguarda para que o buffer seja enchido pela primeira vez
						

					  while(*buffer_load != GATHERING_DONE || !THREAD_DONE[THREAD_SEARCH]){
						if(tamanho_da_fila(f_sensos) > FILA_MIN){
							omp_set_lock(&MC_copy_lock);
							hold = desenfileirar(f_sensos);
							omp_unset_lock(&MC_copy_lock);
							retorno = adicionar_ht(hash_table,hold,criar_value(0,1,0,0));
						}
						if(tamanho_da_fila(f_antisensos) > FILA_MIN){
							omp_set_lock(&MC_copy_lock);
							hold = desenfileirar(f_antisensos);
							omp_unset_lock(&MC_copy_lock);							
							retorno = adicionar_ht(hash_table,hold,criar_value(0,0,1,0));
						}
					  }
						
						while(tamanho_da_fila(f_sensos) > 0){
							omp_set_lock(&MC_copy_lock);
							hold = desenfileirar(f_sensos);
							omp_unset_lock(&MC_copy_lock);
							
							retorno = adicionar_ht(hash_table,hold,criar_value(0,1,0,0));
						}
						while(tamanho_da_fila(f_antisensos) > 0){
							omp_set_lock(&MC_copy_lock);
							hold = desenfileirar(f_antisensos);
							omp_unset_lock(&MC_copy_lock);
							
							retorno = adicionar_ht(hash_table,hold,criar_value(0,0,1,0));
						}
						
					THREAD_DONE[THREAD_CLEANER] = TRUE;
					return;
					//////////////////////////////////////////
					//////////////////////////////////////////
					//////////////////////////////////////////
}


GHashTable* cudaIteracoes(const int bloco1, const int bloco2, const int seqSize_an,const int seqSize_bu,short int **d_matrix_senso,short int **d_matrix_antisenso){
	
	
	Buffer buffer;
	int blocoV = seqSize_bu - bloco1 - bloco2;
	int i;
	int processadas;
	int buffer_load;
	Fila *f_sensos;
	Fila *f_antisensos;
	Fila *founded;
	Fila *tipo_founded;
	cudaStream_t stream1;
	cudaStream_t stream2;
	GHashTable* hash_table;
	char **h_data;
	char **d_data;
	
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
	omp_init_lock(&DtH_copy_lock);
	omp_init_lock(&MC_copy_lock);
	cudaMalloc((void**)&data,buffer_size*sizeof(char*));
	
	//h_data = (char**)malloc(buffer_size*sizeof(char*));
	cudaHostAlloc((void**)&h_data,buffer_size*sizeof(char*),cudaHostAllocDefault);
	for(i=0;i<buffer_size;i++)
		//h_data[i] = (char*)malloc((seqSize_an+1)*sizeof(char));
		cudaHostAlloc((void**)&h_data[i],(seqSize_an+1)*sizeof(char),cudaHostAllocDefault);
		
					
	cudaHostAlloc((void**)&d_data,buffer_size*sizeof(char*),cudaHostAllocDefault);
	for(i=0;i<buffer_size;i++)
		cudaMalloc((void**)&d_data[i],(seqSize_an+1)*sizeof(char));
	
		
	#pragma omp parallel num_threads(OMP_NTHREADS) shared(hash_table) shared(buffer) shared(h_data) shared(d_data) shared(f_sensos) shared(f_antisensos) shared(buffer_load) shared(founded) shared(stream1) shared(stream2) shared(tipo_founded)
	{		
		#pragma omp sections
		{
			#pragma omp section
			{
				buffer_manager(&buffer_load,seqSize_an,h_data,d_data,stream1);
			}
			#pragma omp section
			{
				search_manager(&buffer_load,&processadas,tipo_founded,founded,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,stream1,stream2,d_matrix_senso,d_matrix_antisenso,h_data,d_data,f_sensos,f_antisensos);
			}		
			//#pragma omp section
			//{
				//results_manager(&buffer_load,processadas,tipo_founded,founded,f_sensos,f_antisensos);
			//}
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






void setup_for_cuda(char *seq,short int **d_matrix_senso,short int **d_matrix_antisenso){
	// Recebe um vetor de caracteres com o padrão a ser procurado
	// As matrizes precisam estar alocadas
	char *senso;
	char *antisenso;
	int **h_matrix_senso;
	int **h_matrix_antisenso;
	int size = strlen(seq);
	int i;
	
	h_matrix_senso = (int**)malloc(size*sizeof(short int*));
	h_matrix_antisenso = (int**)malloc(size*sizeof(short int*));
	
	// Aloca memória na GPU
	for(i = 0; i < size ; i++){
		cudaMalloc((void**)&h_matrix_senso[i],N_COL*sizeof(short int));
		cudaMalloc((void**)&h_matrix_antisenso[i],N_COL*sizeof(short int));
	}
	cudaMalloc((void**)&senso,(size+1)*sizeof(char));
	cudaMalloc((void**)&antisenso,(size+1)*sizeof(char));
	
	// Copia dados
    cudaMemcpy(d_matrix_senso,h_matrix_senso,size*sizeof(short int*),cudaMemcpyHostToDevice);
    cudaMemcpy(d_matrix_antisenso,h_matrix_antisenso,size*sizeof(short int*),cudaMemcpyHostToDevice);
      
    cudaMemcpy(senso,seq,(size+1)*sizeof(char),cudaMemcpyHostToDevice);
    cudaMemcpy(antisenso,(const void*)get_antisenso(seq),(size+1)*sizeof(char),cudaMemcpyHostToDevice);
    
    //Configura grafos direto na memória da GPU
	set_grafo_helper(senso,antisenso,d_matrix_senso,d_matrix_antisenso);
	printString("Grafo de busca contigurado.",NULL);
	cudaFree(senso);
	cudaFree(antisenso);
	return;
}

GHashTable* auxCUDA(char *c,const int bloco1, const int bloco2,const int seqSize_bu,gboolean verb,gboolean sil){
	GHashTable* hash_table;
	float tempo;
	int seqSize_an;//Tamanho das sequencias analisadas
	
	printf("CUDA Mode.\n");
	
	verbose = verb;
	silent = sil;
	
	get_setup(&seqSize_an);
    
	// Aloca memória na CPU
	cudaMalloc((void**)&d_matrix_senso,seqSize_an*sizeof(short int*));
	cudaMalloc((void**)&d_matrix_antisenso,seqSize_an*sizeof(short int*));
	//Inicializa
	setup_for_cuda(c,d_matrix_senso,d_matrix_antisenso);
	
	printString("Dados inicializados.\n",NULL);
	printSet(seqSize_an);
	printString("Iniciando iterações:\n",NULL);
	
	hash_table = cudaIteracoes(bloco1,bloco2,seqSize_an,seqSize_bu,d_matrix_senso,d_matrix_antisenso);
    
	cudaThreadExit();
	
	return hash_table;
}
