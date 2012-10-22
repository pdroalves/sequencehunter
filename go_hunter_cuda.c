#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "log.h"
#include "fila.h"

omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
__constant__ char *d_buffer[buffer_size];
const char tmp_cuda_s_name[11] = "tmp_sensos";
const char tmp_cuda_as_name[15] = "tmp_antisensos";
omp_lock_t DtH_copy_lock;

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
				//cudaMemcpyAsync(d_seqs[i],h_seqs[i],(seq_size)*sizeof(char),cudaMemcpyHostToDevice,stream);
			cudaMemcpyAsync(data,d_seqs,loaded*sizeof(char*),cudaMemcpyHostToDevice,stream);	
		}		
		*load = loaded;	
	}
		
	return;
}

void cudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t){
	
	
	Buffer buffer;
	int blocoV = blocos - bloco1 - bloco2+1;
	int i;
	int seqsToProcess;
	int processadas;
	int buffer_load;
	int *h_resultados;
	Fila *f_sensos;
	Fila *f_antisensos;
	char **h_data;
	char **d_data;
	char **founded;		
	cudaStream_t stream1;
	cudaStream_t stream2;

	//Inicializa buffer
	cudaStreamCreate(&stream1);
	cudaStreamCreate(&stream2);
	cudaHostAlloc((void**)&founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
	for(i=0;i<buffer_size;i++)
		cudaHostAlloc((void**)&founded[i],(blocoV+1)*sizeof(char),cudaHostAllocDefault);
	
	buffer_load = 0;
	seqsToProcess = 0;
	processadas=0;
	f_sensos = criar_fila();
	f_antisensos = criar_fila();
	start_fila_lock();
	omp_init_lock(&DtH_copy_lock);
	cudaMalloc((void**)&data,buffer_size*sizeof(char*));
	cudaHostAlloc((void**)&h_data,buffer_size*sizeof(char*),cudaHostAllocMapped | cudaHostAllocWriteCombined);
	for(i=0;i<buffer_size;i++)
		cudaHostAlloc((void**)&h_data[i],(n+1)*sizeof(char),cudaHostAllocMapped | cudaHostAllocWriteCombined);
	cudaHostAlloc((void**)&d_data,buffer_size*sizeof(char*),cudaHostAllocMapped | cudaHostAllocWriteCombined);	
	cudaHostAlloc((void**)&h_resultados,buffer_size*sizeof(int),cudaHostAllocDefault);	
		
	#pragma omp parallel num_threads(4) shared(buffer) shared(f_sensos) shared(f_antisensos) shared(buffer_load) shared(founded) shared(stream1) shared(stream2) shared(seqsToProcess) shared(h_resultados)
	{	
		
		#pragma omp sections
		{
			#pragma omp section
			{
				//////////////////////////////////////////
				// Carrega o buffer //////////////////////
				//////////////////////////////////////////
					while(buffer_load != GATHERING_DONE){//Looping até o final do buffer
					//printf("%d.\n",buffer.load);
					if(buffer_load == 0){
						load_buffer_CUDA(h_data,d_data,n,&buffer_load,stream1);
					}	
				}
				//////////////////////////////////////////
				//////////////////////////////////////////
				//////////////////////////////////////////	
			}
			#pragma omp section
			{
				int num_blocks;
				int num_threads;
				char **d_founded;
				char **dp_founded;	
				/*if(buffer_size >= 512){
					num_blocks = buffer_size/512 +1;
					num_threads = 512;
				}else{*/
					num_blocks = 1;
					num_threads = buffer_size;
				//}
				int *d_resultados;
				cudaMalloc((void**)&d_resultados,buffer_size*sizeof(int));
				cudaHostAlloc((void**)&d_founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
				cudaMalloc((void**)&dp_founded,buffer_size*sizeof(char*));
				for(i=0;i<buffer_size;i++)
					cudaMalloc((void**)&d_founded[i],(blocoV+1)*sizeof(char));
				cudaMemcpyAsync(dp_founded,d_founded,buffer_size*sizeof(char*),cudaMemcpyHostToDevice,stream1);
	
				while( buffer_load == 0){
				}//Aguarda para que o buffer seja enchido pela primeira vez
				while( buffer_load != GATHERING_DONE){
				//Realiza loop enquanto existirem sequências para encher o buffer
						k_busca(num_threads,num_blocks,buffer_load,bloco1,bloco2,blocos,data,d_resultados,dp_founded,d_a,d_c,d_g,d_t,stream1);//Kernel de busca
						omp_set_lock(&DtH_copy_lock);
						// Inicia processamento dos resultados
						//printf("%d\n",p);
						cudaMemcpyAsync(h_resultados,d_resultados,buffer_size*sizeof(int),cudaMemcpyDeviceToHost,stream2);
						checkCudaError();
						for(i=0;i<buffer_load;i++)
							if(h_resultados[i] != 0)
								cudaMemcpyAsync(founded[i],d_founded[i],(blocoV+1)*sizeof(char),cudaMemcpyDeviceToHost,stream2);
						checkCudaError();
						processadas += buffer_load;	
						seqsToProcess = buffer_load;	
						omp_unset_lock(&DtH_copy_lock);
						cudaStreamSynchronize(stream2);
						while(buffer_load==0){}
				}//Aguarda para que o buffer seja enchido pela primeira vez
				
				cudaFreeHost(d_resultados);
			}		
			#pragma omp section
			{
				//////////////////////////////////////////
				// Realiza as iteracoes///////////////////
				//////////////////////////////////////////
				char *tmp;
				
				while( seqsToProcess == 0){
				}//Aguarda para que o buffer seja enchido pela primeira vez
				
				while(buffer_load != GATHERING_DONE){
						omp_set_lock(&DtH_copy_lock);
						// Essa parte pode ser feita em paralelo
						for(i = 0; i < seqsToProcess;i++){//Copia sequências senso e antisenso encontradas
							switch(h_resultados[i]){
								case SENSO:
									tmp = founded[i];
									if(verbose == TRUE && silent != TRUE)	
										printf("S: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_sensos));
									enfileirar(f_sensos,tmp);
									seqsToProcess--;
									buffer_load--;
								break;
								case ANTISENSO:
									tmp = founded[i];
									if(verbose == TRUE && silent != TRUE)
										printf("N: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_antisensos));
									enfileirar(f_antisensos,get_antisenso(tmp));
									seqsToProcess--;
									buffer_load--;
								break;
								default:
									seqsToProcess--;
									buffer_load--;
								break;
							}
						}
						omp_unset_lock(&DtH_copy_lock);
											
						/*if(seqsToProcess != 0 || buffer_load !=0)
						{
							//printf("Erro! Buffer não foi totalmente esvaziado.\n");
							seqsToProcess = 0;
							buffer_load = 0;
						}*/
						
						while(seqsToProcess==0 && buffer_load != GATHERING_DONE){}
										
					}
				}
				#pragma omp section
				{
					//////////////////////////////////////////
					// Libera memoria ////////////////////////
					//////////////////////////////////////////
					  FILE *tmp_sensos;
					  FILE *tmp_antisensos;
					  
					  tmp_sensos = fopen(tmp_cuda_s_name,"w");
					  tmp_antisensos = fopen(tmp_cuda_as_name,"w");
					  
					  while( buffer_load == 0){
						}//Aguarda para que o buffer seja enchido pela primeira vez
						

					  while(buffer_load != GATHERING_DONE){
						if(tamanho_da_fila(f_sensos) > 0){
							despejar_seq(desenfileirar(f_sensos),tmp_sensos);
						}
						if(tamanho_da_fila(f_antisensos) > 0){
							despejar_seq(desenfileirar(f_antisensos),tmp_antisensos);
						}
					  }
						
						if(tamanho_da_fila(f_sensos) > 0){
						  despejar_fila(f_sensos,tmp_sensos);
						}
						if(tamanho_da_fila(f_antisensos) > 0){
						  despejar_fila(f_antisensos,tmp_antisensos);
						}
						
					  
					  fclose(tmp_sensos);
					  fclose(tmp_antisensos);
					//////////////////////////////////////////
					//////////////////////////////////////////
					//////////////////////////////////////////
				}
		}
	}
	//printf("Iterações executadas: %d.\n",iter);
	//free(tmp);
	cudaDeviceReset();
	//cudaStreamDestroy(stream1);
	//cudaStreamDestroy(stream2);
	/*for(i=0;i<buffer_size;i++){
		cudaFreeHost(founded[i]);
		cudaFreeHost(h_data[i]);
	}	
	cudaFreeHost(founded);
	cudaFreeHost(h_data);
	cudaFreeHost(d_data);*/
	//cudaFree(data);
	return;
}

void setup_for_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t){
	//Recebe um vetor de caracteres com o padrão a ser procurado
	//Recebe ponteiros para os quatro vértices do grafo já na memória da GPU
	char *d_senso;
	char *d_antisenso;
	int size = strlen(seq)+1;
	
	//Aloca memória na GPU
    cudaMalloc((void**)&d_senso,size*sizeof(char));
    cudaMalloc((void**)&d_antisenso,size*sizeof(char));
    
    cudaMemcpy(d_senso,seq,size*sizeof(char),cudaMemcpyHostToDevice);
    cudaMemcpy(d_antisenso,(const void*)get_antisenso(seq),size*sizeof(char),cudaMemcpyHostToDevice);
    
    //Configura grafos direto na memória da GPU
	set_grafo_helper(d_senso,d_antisenso,d_a,d_c,d_g,d_t);
	printString("Grafo de busca contigurado.",NULL);
	cudaFree(d_senso);
	cudaFree(d_antisenso);
	return;
}

void auxCUDA(char *c,const int bloco1,const int bloco2,const int blocos,gboolean verb,gboolean sil){
	printf("CUDA Mode.\n");
	int n;//Elementos por sequência
	vgrafo *d_a;
	vgrafo *d_c;
	vgrafo *d_g;
	vgrafo *d_t;
	cudaEvent_t start;
	cudaEvent_t stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	float tempo;
	
	verbose = verb;
	silent = sil;
	
	get_setup(&n);
	
	cudaMalloc((void**)&d_a,sizeof(vgrafo));
    cudaMalloc((void**)&d_c,sizeof(vgrafo));
    cudaMalloc((void**)&d_g,sizeof(vgrafo));
    cudaMalloc((void**)&d_t,sizeof(vgrafo));
    
	//Inicializa
	setup_for_cuda(c,d_a,d_c,d_g,d_t);
	
	printString("Dados inicializados.\n",NULL);
	printSet(n);
	printString("Iniciando iterações:\n",NULL);
	
   // cudaEventRecord(start,0);
    cudaIteracoes(bloco1,bloco2,blocos,n,d_a,d_c,d_g,d_t);
   // cudaEventRecord(stop,0);
   // cudaEventSynchronize(stop);
   // cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	cudaFree(d_a);
	cudaFree(d_c);
	cudaFree(d_g);
	cudaFree(d_t);
	cudaThreadExit();
	return;

}
