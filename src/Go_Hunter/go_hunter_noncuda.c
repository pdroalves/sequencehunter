#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "../Headers/hashtable.h"
#include "../Headers/estruturas.h"
#include "../Headers/go_hunter_noncuda.h"
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

#define MIN_LEN_TO_PRINT 100000

gboolean THREAD_DONE[3];
omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
gboolean debug;
gboolean central_cut;
gboolean regiao_5l;
gboolean gui_run;
int dist_regiao_5l;
int tam_regiao_5l;
omp_lock_t MC_copy_lock;

const int buffer_size_NC = buffer_size;
const char tmp_ncuda_s_name[11] = "tmp_sensos";
const char tmp_ncuda_as_name[15] = "tmp_antisensos";

void load_buffer_NONCuda(Buffer *b,int n){
	
	if(b->load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
		b->load = fill_buffer(b->seq,b->capacidade);//Enche o buffer e guarda a quantidade de sequências carregadas.			
	}
		
	
	return;
}

void nc_buffer_manager(Buffer *b,int n){
		//////////////////////////////////////////
		// Carrega o buffer //////////////////////
		//////////////////////////////////////////
				THREAD_DONE[THREAD_BUFFER_LOADER] = FALSE;
			while(b->load != -1){//Looping até o final do buffer
				//printf("%d.\n",buffer.load);
				if(b->load == 0)
					load_buffer_NONCuda(b,n);
			}
		
		THREAD_DONE[THREAD_BUFFER_LOADER] = TRUE;
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////	
}

void nc_search_manager(Buffer *buffer,int bloco1,int bloco2,int blocos,const int seqSize_an,Fila *toStore){
	//////////////////////////////////////////
		// Realiza as iteracoes///////////////////
		//////////////////////////////////////////
		
		int *search_gaps;
		int *resultados;
		gboolean retorno;
		int gap;
		int tam;
		int i;
		int p;
		int diff;
		char *central;
		char *cincol;
		char *seqToSave;
		cudaEvent_t startK,stopK,start,stop;
		float elapsedTimeK,elapsedTime;
		float iteration_time;
		int fsensos,fasensos;
		const int blocoV = blocos-bloco1-bloco2;
		
		THREAD_DONE[THREAD_SEARCH] = FALSE;
		p = 0;
		fsensos=fasensos=0;
		
		resultados = (int*)malloc(buffer_size_NC*sizeof(int));
		search_gaps = (int*)malloc(buffer_size_NC*sizeof(int));
				
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventCreate(&startK);
		cudaEventCreate(&stopK);
		
		iteration_time = 0;
		diff = 0;
		
			while( buffer->load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
				cudaEventRecord(start,0);
			while(buffer->load != GATHERING_DONE || 
					THREAD_DONE[THREAD_BUFFER_LOADER] == FALSE){
				//Realiza loop enquanto existirem sequências para encher o buffer
				cudaEventRecord(stop,0);
				cudaEventSynchronize(stop);
				cudaEventElapsedTime(&elapsedTime,start,stop);
				iteration_time += elapsedTime;
				//if(verbose == TRUE && silent != TRUE)	
				//	printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
				if(debug){
					if(!silent)
					printf("Tempo até retornar para busca em %f ms\n",elapsedTime);
					printString("Retorno da busca:\n",NULL);
					//print_tempo_optional(elapsedTime);
				}
				cudaEventRecord(start,0);

				cudaEventRecord(startK,0);
					busca(bloco1,bloco2,blocos,buffer,resultados,search_gaps);//Kernel de busca					
				cudaEventRecord(stopK,0);
				cudaEventSynchronize(stopK);

				cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
				iteration_time += elapsedTimeK;
				if(debug){
					if(!silent)
					printf("Execucao da busca em %f ms\n",elapsedTimeK);
					printString("Execucao da busca:\n",NULL);
					//print_tempo_optional(elapsedTimeK);
				}
				cudaEventRecord(start,0);
						
					
				tam = buffer->load;
				p += tam;
					
				for(i = 0; i < tam;i++){
					//Copia sequências senso e antisenso encontradas
						switch(resultados[i]){
							case SENSO:
								central = (char*)malloc((seqSize_an+1)*sizeof(char));
								if(central_cut){
									gap = search_gaps[i];
									strncpy(central,buffer->seq[i]+gap,blocoV);
									central[blocoV] = '\0';
								}else{									
									strncpy(central,buffer->seq[i],seqSize_an+1);
								}


								if(regiao_5l){
									cincol = (char*)malloc((blocos+1)*sizeof(char));

									gap = search_gaps[i] - dist_regiao_5l;
									strncpy(cincol,buffer->seq[i] + gap,tam_regiao_5l);
									cincol[tam_regiao_5l] = '\0';
								}
								
							fsensos++;
										if(regiao_5l)
											enfileirar(toStore,central,cincol,SENSO);
										else
											enfileirar(toStore,central,NULL,SENSO);

								buffer->load--;
							break;
							case ANTISENSO:
								central = (char*)malloc((seqSize_an+1)*sizeof(char));
								if(central_cut){
									gap = search_gaps[i];
									strncpy(central,buffer->seq[i]+gap,blocoV);
									central[blocoV] = '\0';
								}else{									
									strncpy(central,buffer->seq[i],seqSize_an+1);
								}

								
								if(regiao_5l){
									cincol = (char*)malloc((blocos+1)*sizeof(char));
									
									gap = search_gaps[i] + dist_regiao_5l-1;
									strncpy(cincol,buffer->seq[i] + gap,tam_regiao_5l);
									cincol[tam_regiao_5l] = '\0';
								}

								fasensos++;
										if(regiao_5l)
											enfileirar(toStore,get_antisenso(central),get_antisenso(cincol),ANTISENSO);
										else
											enfileirar(toStore,get_antisenso(central),NULL,ANTISENSO);
								buffer->load--;
							break;
							default:
								buffer->load--;
							break;
						}
					}
					
					if(verbose && !silent)		
						printf("Sequencias processadas: %d - S: %d, AS: %d\n",p,fsensos,fasensos);
					diff+= p;
					if(gui_run && diff > MIN_LEN_TO_PRINT){
							printf("T%dS%dAS%d\n",p,fsensos,fasensos);
							diff = 0;
					}
					
					
					while(buffer->load==0){}
									
			}
				
			
	  if(!silent)
		printf("Busca realizada em %.2f ms.\n",iteration_time);
	
		THREAD_DONE[THREAD_SEARCH] = TRUE;
		return;
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
		
}

void nc_queue_manager(Fila *toStore){
	FilaItem *hold;
	
	while(!THREAD_DONE[THREAD_SEARCH]){
		if(tamanho_da_fila(toStore) > 0){
			hold = desenfileirar(toStore);
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



void NONcudaIteracoes(int bloco1,int bloco2,int blocos,const int seqSize_an){
	
	Buffer buffer;
	Fila *toStore;
	int blocoV;
	//Inicializa
	blocoV = blocos - bloco1 - bloco2+1;
	prepare_buffer(&buffer,buffer_size_NC);	
	criar_ghash_table();
	toStore = criar_fila("toStore");
			
	#pragma omp parallel num_threads(OMP_NTHREADS) shared(buffer) shared(toStore)
	{	
		
		#pragma omp sections
		{
			#pragma omp section
			{
				nc_buffer_manager(&buffer,seqSize_an);
			}
			#pragma omp section
			{
				nc_search_manager(&buffer,bloco1,bloco2,blocos,seqSize_an,toStore);
			}
			#pragma omp section
			{
				nc_queue_manager(toStore);
			}
		}
	}
	
	return;
}


void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,Params set){
	
	int seqSize_an;//Elementos por sequência
	//Arrumar nova maneira de contar o tempo sem usar a cuda.h
	//cudaEvent_t start;
	//cudaEvent_t stop;
	//cudaEventCreate(&start);
	//cudaEventCreate(&stop);
	float tempo;

	tempo = 0;
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

	  if(!silent)
	printf("OpenMP Mode.\n");
	printString("OpenMP Mode.\n",NULL);
	printf("Buffer size: %d\n",buffer_size);
	printStringInt("Buffer size: ",buffer_size);
	get_setup(&seqSize_an);
	
	setup_without_cuda(c);
	
	printString("Dados inicializados.\n",NULL);
	printSet(seqSize_an);
	printString("Iniciando iterações:\n",NULL);
	
    //cudaEventRecord(start,0);
	NONcudaIteracoes(bloco1,bloco2,blocos,seqSize_an);
    //cudaEventRecord(stop,0);
    //cudaEventSynchronize(stop);
    //cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	
return;	
}

