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
				cudaMemcpyAsync(d_seqs[i],h_seqs[i],(seq_size)*sizeof(char),cudaMemcpyHostToDevice,stream);
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
	int tam;
	int processadas;
	int buffer_load;
	Fila *f_sensos;
	Fila *f_antisensos;
	char **h_data;
	char **d_data;
	
	//Inicializa buffer
	buffer_load = 0;
	processadas=0;
	f_sensos = criar_fila();
	f_antisensos = criar_fila();
	start_fila_lock();
	cudaMalloc((void**)&data,buffer_size*sizeof(char*));
	cudaHostAlloc((void**)&h_data,buffer_size*sizeof(char*),cudaHostAllocMapped | cudaHostAllocWriteCombined);
	for(i=0;i<buffer_size;i++)
		cudaHostAlloc((void**)&h_data[i],(n+1)*sizeof(char),cudaHostAllocMapped | cudaHostAllocWriteCombined);
	cudaHostAlloc((void**)&d_data,buffer_size*sizeof(char*),cudaHostAllocMapped | cudaHostAllocWriteCombined);
	for(i=0;i<buffer_size;i++)
		cudaMalloc((void**)&d_data[i],(n+1)*sizeof(char));
		
		
	#pragma omp parallel num_threads(3) shared(buffer) shared(f_sensos) shared(f_antisensos) shared(buffer_load)
	{	
		
		#pragma omp sections
		{
		#pragma omp section
		{
		//////////////////////////////////////////
		// Carrega o buffer //////////////////////
		//////////////////////////////////////////
				cudaStream_t stream;
				cudaStreamCreate(&stream);
				while(buffer_load != -1){//Looping até o final do buffer
				//printf("%d.\n",buffer.load);
				if(buffer_load == 0)
					load_buffer_CUDA(h_data,d_data,n,&buffer_load,stream);
			}
		
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////	
		}
		#pragma omp section
		{
			//////////////////////////////////////////
			// Realiza as iteracoes///////////////////
			//////////////////////////////////////////
			int *hold;
			int *resultados;
			int num_blocks;
			int num_threads;
			num_blocks = 1;
			num_threads = buffer_size;
			char *tmp;
			char **founded;
			cudaStream_t stream;
					
			cudaHostAlloc((void**)&hold,buffer_size*sizeof(int),cudaHostAllocDefault);
			cudaHostAlloc((void**)&resultados,buffer_size*sizeof(int),cudaHostAllocDefault);
			cudaHostAlloc((void**)&founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
			for(i=0;i<buffer_size;i++)
				cudaHostAlloc((void**)&founded[i],(blocoV+1)*sizeof(char),cudaHostAllocDefault);
			cudaStreamCreate(&stream);
			
			while( buffer_load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
			while(buffer_load != -1){
					//Realiza loop enquanto existirem sequências para encher o buffer
					k_busca(buffer_load,num_blocks,num_threads,bloco1,bloco2,blocos,data,resultados,d_a,d_c,d_g,d_t,stream);//Kernel de busca
					
					// Inicia processamento dos resultados
					tam = buffer_load;
					processadas += tam;
					//printf("%d\n",p);
					cudaMemcpyAsync(hold,resultados,buffer_size*sizeof(int),cudaMemcpyDeviceToHost,stream);
					checkCudaError();
					for(i=0;i<buffer_size;i++)
						if(hold[i] != 0)
							cudaMemcpyAsync(founded[i],d_data[i],(blocoV+1)*sizeof(char),cudaMemcpyDeviceToHost,stream);
					checkCudaError();		
					
					// Essa parte pode ser feita em paralelo
					for(i = 0; i < tam;i++){//Copia sequências senso e antisenso encontradas
						switch(hold[i]){
							case SENSO:
								tmp = founded[i];
								if(verbose == TRUE && silent != TRUE)	
									printf("S: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_sensos));
								enfileirar(f_sensos,tmp);
								buffer_load--;
							break;
							case ANTISENSO:
								tmp = founded[i];
								if(verbose == TRUE && silent != TRUE)
									printf("N: %s - %d - F: %d\n",tmp,processadas,tamanho_da_fila(f_antisensos));
								enfileirar(f_antisensos,get_antisenso(tmp));
								buffer_load--;
							break;
							default:
								buffer_load--;
							break;
						}
					}
					
										
					if(buffer_load != 0)
					{
						printf("Erro! Buffer não foi totalmente esvaziado.\n");
						buffer_load = 0;
					}
					
					while(buffer_load==0){}
									
				}
				cudaFree(resultados);
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
				

			  while(buffer_load != -1){
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
