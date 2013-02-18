#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "../Headers/hashtable.h"
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"
#include "../Headers/operacoes.h"
#include "../Headers/busca.h"
#include "../Headers/log.h"
#include "../Headers/fila.h"

#define OMP_NTHREADS 3
#define THREAD_BUFFER_LOADER 0
#define THREAD_SEARCH 1
#define THREAD_QUEUE 2
#define buffer_size 4096 // Capacidade máxima do buffer

omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
gboolean debug;
gboolean central_cut;
gboolean regiao_5l;
gboolean gui_run;
int dist_regiao_5l;
int tam_regiao_5l;

char **data;
char **h_data;
gboolean THREAD_DONE[OMP_NTHREADS];

int load_buffer_CUDA(char **h_seqs,char **d_seqs,int seq_size,cudaStream_t stream){
	int i;
	int loaded;
	
	loaded = fill_buffer(h_seqs,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
	if(loaded != GATHERING_DONE){
		
		//Copia sequencias para GPU
		for(i=0;i<loaded;i++)
			//cudaHostGetDevicePointer(&d_seqs[i],h_seqs[i],0);
			cudaMemcpyAsync(d_seqs[i],h_seqs[i],(seq_size+1)*sizeof(char),cudaMemcpyHostToDevice,stream);
	}		
	
	
	return loaded;
}

void buffer_manager(int *buffer_load,
						int n,
						cudaStream_t stream1){
							
								
								// Carrega o buffer //////////////////////
				//////////////////////////////////////////
				int i;
				
				// Buffer 1
				/*cudaHostAlloc((void**)&h_data,buffer_size*sizeof(char*),cudaHostAllocDefault);
				for(i=0;i<buffer_size;i++)
					cudaHostAlloc((void**)&h_data[i],(n+1)*sizeof(char),cudaHostAllocDefault);	*/
					
				h_data = (char**)malloc(buffer_size*sizeof(char*));
				for(i=0;i<buffer_size;i++)
								h_data[i] = (char*)malloc((n+1)*sizeof(char));
								
				cudaHostAlloc((void**)&data,buffer_size*sizeof(char*),cudaHostAllocDefault);
				for(i=0;i<buffer_size;i++)
					cudaMalloc((void**)&data[i],(n+1)*sizeof(char));
		//////////////////////////////////////////
		// Carrega o buffer //////////////////////
		//////////////////////////////////////////
				THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
			while(*buffer_load != -1){//Looping até o final do buffer
				//printf("%d.\n",buffer.load);
				if(*buffer_load == 0)
							*buffer_load = load_buffer_CUDA(h_data,data,n,stream1);
			}
		
		THREAD_DONE[THREAD_BUFFER_LOADER] = TRUE;
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////	
}

void search_manager(int *buffer_load,
							int *processadas,
							Fila *toStore,
							const int seqSize_an,
							const int seqSize_bu,
							int bloco1,
							int bloco2,
							int blocoV,
							cudaStream_t stream1,
							cudaStream_t stream2){						
				int i;
				short int *h_resultados;
				short int *d_resultados;
				short int *h_search_gaps;
				short int *d_search_gaps;
				char **h_founded;
				char **d_founded;
				char **d_tmp_founded;
				char *hold_seq;
				char *central;
				char *cincol;
				int loaded;
				int hold;
				int p;
				float iteration_time;
				int fsenso;
				int fasenso;	
				int gap;
				cudaEvent_t startK,stopK;
				cudaEvent_t start,stop;
				cudaEvent_t startV,stopV;
				char **local_data;
				float elapsedTimeK,elapsedTime,elapsedTimeV;
				gboolean retorno;
				
				fsenso=fasenso=0;

				cudaEventCreate(&start);
				cudaEventCreate(&stop);
				cudaEventCreate(&startK);
				cudaEventCreate(&stopK);
				cudaEventCreate(&startV);
				cudaEventCreate(&stopV);
				
				// Alloc: resultados de cada iteracao				
				h_resultados = (short int*)malloc(buffer_size*sizeof(short int));
				h_search_gaps = (short int*)malloc(buffer_size*sizeof(short int));
				cudaMalloc((void**)&d_resultados,buffer_size*sizeof(short int));
				cudaMalloc((void**)&d_search_gaps,buffer_size*sizeof(short int));
				
				// Alloc: Seqs encontradas
				//CPU
				cudaHostAlloc((void**)&h_founded,buffer_size*sizeof(char*),cudaHostAllocDefault);
				for(i=0;i<buffer_size;i++)
					cudaHostAlloc((void**)&h_founded[i],(seqSize_an+1)*sizeof(char),cudaHostAllocDefault);
				
				//GPU	
				d_tmp_founded = (char**)malloc(buffer_size*sizeof(char*));
				for(i=0;i<buffer_size;i++)
					cudaHostAlloc((void**)&d_tmp_founded[i],(seqSize_an+1)*sizeof(char),cudaHostAllocDefault);
					
				cudaMalloc((void**)&d_founded,buffer_size*sizeof(char**));
				cudaMemcpy(d_founded,d_tmp_founded,buffer_size*sizeof(char*),cudaMemcpyHostToDevice);
				
				// Alloc: Vetor temporario
				local_data = (char**)malloc(buffer_size*sizeof(char*));
				for(i=0;i<buffer_size;i++)
					local_data[i] = (char*)malloc((seqSize_an+1)*sizeof(char));
				hold_seq = (char*)malloc(seqSize_an*sizeof(char));
				
				iteration_time = 0;
				
				while( *buffer_load == 0){
				}//Aguarda para que o buffer seja enchido pela primeira vez
				
						cudaEventRecord(start,0);
				while( *buffer_load != GATHERING_DONE){
					//Realiza loop enquanto existirem sequencias para encher o buffer
						cudaEventRecord(stop,0);
						cudaEventSynchronize(stop);
						
						cudaEventElapsedTime(&elapsedTime,start,stop);
						if(debug&&!silent)
							printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
						iteration_time += elapsedTime;
						
						loaded = *buffer_load;
						
						// Execuca iteracao
						cudaEventRecord(startK,0);
						k_busca(*buffer_load,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,data,d_resultados,d_search_gaps,d_founded,stream2);//Kernel de busca
						cudaEventRecord(stopK,0);						
						cudaEventSynchronize(stopK);
						cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
						if(debug&&!silent)
							printf("Execucao da busca em %.2f ms\n",elapsedTimeK);
						
						// Guarda tempo gasto na iteracao					
						iteration_time += elapsedTimeK;
						
						
						cudaEventRecord(start,0);
						
						// Inicia processamento dos resultados
						*processadas += loaded;
						cudaStreamSynchronize(stream2);
						
						// Em casos reais, cada iteracao possuirah poucos eventos. Portanto, a copia de dados para 
						// o host serah bastante casual e esse trecho nao deve implicar em perda de desempenho.	
						cudaMemcpy(h_resultados,d_resultados,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost);
						cudaMemcpy(h_search_gaps,d_search_gaps,buffer_size*sizeof(short int),cudaMemcpyDeviceToHost);
						//for(i=0;i<buffer_size;i++)
						//	if(h_resultados[i] != 0)
						//		cudaMemcpyAsync(h_founded[i],h_data[i],seqSize_an*sizeof(char),cudaMemcpyDeviceToHost,stream2);
						
						//cudaStreamSynchronize(stream2);
						
						
						// Guarda o que foi encontrado
						for(i=0;i<loaded;i++)
							if(h_resultados[i] != 0){
								switch(h_resultados[i]){		
									case SENSO:									
										central = (char*)malloc((seqSize_an+1)*sizeof(char));
										if(central_cut){
											gap = h_search_gaps[i];
											strncpy(central,h_data[i]+gap,blocoV);
											central[blocoV] = '\0';
										}else{									
											strncpy(central,h_data[i],seqSize_an+1);
										}

										if(regiao_5l){
											cincol = (char*)malloc((seqSize_an+1)*sizeof(char));

											gap = h_search_gaps[i] - dist_regiao_5l;
											strncpy(cincol,h_data[i] + gap,tam_regiao_5l);
											cincol[tam_regiao_5l] = '\0';
										}
								
										fsenso++;
										if(regiao_5l)
											enfileirar(toStore,central,cincol,SENSO);
										else
											enfileirar(toStore,central,NULL,SENSO);
									break;
									case ANTISENSO:
										central = (char*)malloc((seqSize_an+1)*sizeof(char));
										if(central_cut){
											gap = h_search_gaps[i];
											strncpy(central,h_data[i]+gap,blocoV);
											central[blocoV] = '\0';
										}else{									
											strncpy(central,h_data[i],seqSize_an+1);
										}

								
										if(regiao_5l){
											cincol = (char*)malloc((seqSize_an+1)*sizeof(char));
											gap = h_search_gaps[i] + dist_regiao_5l-1;
											strncpy(cincol,h_data[i] + gap,tam_regiao_5l);
											cincol[tam_regiao_5l] = '\0';
										}

										fasenso++;
										if(regiao_5l)											
											enfileirar(toStore,get_antisenso(central),get_antisenso(cincol),ANTISENSO);
										else
											enfileirar(toStore,get_antisenso(central),NULL,ANTISENSO);
								
									break;
								}
							}
							
						// Libera para o thread buffer_manager carregar mais sequencias
						*buffer_load = 0;		
						
						checkCudaError();
						if(verbose && !silent)
							printf("Sequencias analisadas: %d - S: %d, AS: %d\n",*processadas,fsenso,fasenso);
						if(gui_run)
							printf("T%dS%dAS%d\n",*processadas,fsenso,fasenso);
						
						if(debug && !silent)
							printf("Fila: %d\n",tamanho_da_fila(toStore));
							
							
						// Aguarda o buffer estar cheio novamente
						cudaEventRecord(startV,0);
						while(*buffer_load == 0 && !THREAD_DONE[THREAD_BUFFER_LOADER]){}
						cudaEventRecord(stopV,0);						
						cudaEventSynchronize(stopV);
						cudaEventElapsedTime(&elapsedTimeV,startV,stopV);
						
						// Evita desincronização
						if(*buffer_load == 0 && THREAD_DONE[THREAD_BUFFER_LOADER]){
							*buffer_load = GATHERING_DONE;
						}
						if(debug && !silent)
							printf("Tempo aguardando encher o buffer: %.2f ms\n",elapsedTimeV);
						
				}
				if(!silent)
				if(iteration_time > 10000)
					printf("Busca realizada em %.2f min.\n",iteration_time/(float)60000);
				else 
					printf("Busca realizada em %.2f ms.\n",iteration_time);
			
				for(i=0;i<buffer_size;i++)
					free(local_data[i]);
				free(local_data);
				cudaFree(d_resultados);
				for(i=0;i<buffer_size;i++)
					cudaFreeHost(h_founded[i]);
				cudaFreeHost(h_founded);
				free(h_resultados);
				cudaEventDestroy(start);
				cudaEventDestroy(stop);
				cudaEventDestroy(startK);
				cudaEventDestroy(stopK);
				THREAD_DONE[THREAD_SEARCH] = TRUE;
				return;
}

void queue_manager(Fila *toStore){
	FilaItem *hold;
	
	while(!THREAD_DONE[THREAD_SEARCH]){
		if(tamanho_da_fila(toStore) > 0){
			hold = desenfileirar(toStore);
			if(hold == NULL){
				printf("Erro alocando memoria.\n");
				printString("Erro alocando memoria.",NULL);
				exit(1);
			}
			if(hold->tipo == SENSO)
				adicionar_ht(hold->seq_central,hold->seq_cincoL,"S");
			else
				adicionar_ht(hold->seq_central,hold->seq_cincoL,"AS");
		    free(hold->seq_central);
		    free(hold->seq_cincoL);
			free(hold);
		}
	}
	
	while(tamanho_da_fila(toStore) > 0){		
			hold = desenfileirar(toStore);
			if(hold == NULL){
				printf("Erro alocando memoria.\n");
				printString("Erro alocando memoria.",NULL);
				exit(1);
			}
			if(hold->tipo == SENSO)
				adicionar_ht(hold->seq_central,hold->seq_cincoL,"S");
			else
				adicionar_ht(hold->seq_central,hold->seq_cincoL,"AS");
		    free(hold->seq_central);
		    free(hold->seq_cincoL);
		   free(hold);
	}
	
	THREAD_DONE[THREAD_QUEUE] = TRUE;
	return;
}

void cudaIteracoes(const int bloco1, const int bloco2, const int seqSize_an,const int seqSize_bu){
	
	
	Buffer buffer;
	int blocoV = seqSize_bu - bloco1 - bloco2;
	int i;
	int processadas;
	int buffer_load;
	Fila *toStore;
	cudaStream_t stream1;
	cudaStream_t stream2;
	
	//Inicializa buffer
	cudaStreamCreate(&stream1);
	cudaStreamCreate(&stream2);

	buffer_load = 0;
	processadas=0;
	toStore = criar_fila("toStore");
	cudaMalloc((void**)&data,buffer_size*sizeof(char*));
	
	
	THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
	THREAD_DONE[THREAD_SEARCH] = FALSE;
	THREAD_DONE[THREAD_QUEUE] = FALSE;
		
	#pragma omp parallel num_threads(OMP_NTHREADS) shared(buffer) shared(buffer_load) shared(stream1) shared(stream2) shared(toStore)
	{		
		#pragma omp sections
		{
			#pragma omp section
			{
				buffer_manager(&buffer_load,seqSize_an,stream1);
			}
			#pragma omp section
			{
				search_manager(&buffer_load,&processadas,toStore,seqSize_an,seqSize_bu,bloco1,bloco2,blocoV,stream1,stream2);
			}
			#pragma omp section
			{
				queue_manager(toStore);
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
	cudaFree(data);
	return;
}

void auxCUDA(char *c,const int bloco1, const int bloco2,const int seqSize_bu,Params set){
	float tempo;
	int seqSize_an;//Tamanho das sequencias analisadas
	verbose = set.verbose;
	silent = set.silent;
	debug = set.debug;
	central_cut = set.cut_central;
	gui_run = set.gui_run;
	dist_regiao_5l = set.dist_regiao_5l;
	tam_regiao_5l = set.tam_regiao_5l;
	if(dist_regiao_5l && tam_regiao_5l)
		regiao_5l = TRUE;
	else
		regiao_5l = FALSE;
	
	if(!silent || gui_run)
	printf("CUDA Mode.\n");
	printString("CUDA Mode.\n",NULL);
	
	printf("Buffer size: %d\n",buffer_size);
	printStringInt("Buffer size: ",buffer_size);
	
	get_setup(&seqSize_an);
	
	//Inicializa
	setup_for_cuda(c);
	
	printString("Dados inicializados.\n",NULL);
	printSet(seqSize_an);
	printString("Iniciando iterações:\n",NULL);
	
	cudaIteracoes(bloco1,bloco2,seqSize_an,seqSize_bu);
    
	cudaThreadExit();
	
	return;
}
