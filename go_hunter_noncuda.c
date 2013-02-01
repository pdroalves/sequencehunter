#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "ghashtable.h"
#include "estruturas.h"
#include "go_hunter_noncuda.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "log.h"
#include "fila.h"

#define OMP_NTHREADS 2
#define THREAD_BUFFER_LOADER 0
#define THREAD_SEARCH 1

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

GHashTable* nc_search_manager(Buffer *buffer,int bloco1,int bloco2,int blocos){
	//////////////////////////////////////////
		// Realiza as iteracoes///////////////////
		//////////////////////////////////////////
		
		GHashTable *hash_table;
		int *search_gaps;
		int *resultados;
		gboolean retorno;
		int gap;
		int tam;
		int i;
		int p;
		char *central;
		char *cincol;
		char *seqToSave;
		cudaEvent_t startK,stopK,start,stop;
		float elapsedTimeK,elapsedTime;
		float iteration_time;
		int fsensos,fasensos;
		const int blocoV = blocos-bloco1-bloco2;
		
		THREAD_DONE[THREAD_SEARCH] = FALSE;
		hash_table = criar_ghash_table();
		p = 0;
		fsensos=fasensos=0;
		
		resultados = (int*)malloc(buffer_size_NC*sizeof(int));
		search_gaps = (int*)malloc(buffer_size_NC*sizeof(int));
				
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventCreate(&startK);
		cudaEventCreate(&stopK);
		
		iteration_time = 0;
		
			while( buffer->load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
				cudaEventRecord(start,0);
			while(buffer->load != GATHERING_DONE && 
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
								central = (char*)malloc((blocos+1)*sizeof(char));
								if(central_cut){
									gap = search_gaps[i];
									strncpy(central,buffer->seq[i]+gap,blocoV);
									central[blocoV] = '\0';
								}else{									
									strncpy(central,buffer->seq[i],blocos);
								}


								if(regiao_5l){
									cincol = (char*)malloc((blocos+1)*sizeof(char));

									gap = search_gaps[i] - dist_regiao_5l;
									strncpy(cincol,buffer->seq[i] + gap,tam_regiao_5l);
									cincol[tam_regiao_5l] = '\0';
								}
								
							fsensos++;
								if(regiao_5l)
									retorno = adicionar_ht(hash_table,central,cincol,criar_value(0,1,0,0));
								else
									retorno = adicionar_ht(hash_table,central,NULL,criar_value(0,1,0,0));

								buffer->load--;
							break;
							case ANTISENSO:
								central = (char*)malloc((blocos+1)*sizeof(char));
								if(central_cut){
									gap = search_gaps[i];
									strncpy(central,buffer->seq[i]+gap,blocoV);
									central[blocoV] = '\0';
								}else{									
									strncpy(central,buffer->seq[i],blocos);
								}

								
								if(regiao_5l){
									cincol = (char*)malloc((blocos+1)*sizeof(char));
									gap = search_gaps[i] + dist_regiao_5l;
									strncpy(cincol,buffer->seq[i] + gap,tam_regiao_5l);
									cincol[tam_regiao_5l] = '\0';
								}

								fasensos++;
								if(regiao_5l)
									retorno = adicionar_ht(hash_table,get_antisenso(central),get_antisenso(cincol),criar_value(0,0,1,0));
								else
									retorno = adicionar_ht(hash_table,get_antisenso(central),NULL,criar_value(0,0,1,0));
								
								buffer->load--;
							break;
							default:
								buffer->load--;
							break;
						}
					}
					
					if(verbose && !silent)		
						printf("Sequencias processadas: %d - S: %d, AS: %d\n",p,fsensos,fasensos);
					if(gui_run)
							printf("T%dS%dAS%d\n",p,fsensos,fasensos);
					
					if(buffer->load != 0)
					{
						if(debug&&!silent)
							printf("Erro! Buffer não foi totalmente esvaziado.\n");
						if(buffer->load != GATHERING_DONE)
							buffer->load = 0;
					}
					
					while(buffer->load==0){}
									
			}
				
			
	  if(!silent)
		printf("Busca realizada em %.2f ms.\n",iteration_time);
	
		THREAD_DONE[THREAD_SEARCH] = TRUE;
		return hash_table;
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
		
}



GHashTable* NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n){
	
	Buffer buffer;
	int blocoV;
	GHashTable* hash_table;
	
	//Inicializa
	blocoV = blocos - bloco1 - bloco2+1;
	prepare_buffer(&buffer,buffer_size_NC);	
			
	#pragma omp parallel num_threads(OMP_NTHREADS) shared(buffer)
	{	
		
		#pragma omp sections
		{
			#pragma omp section
			{
				nc_buffer_manager(&buffer,n);
			}
			#pragma omp section
			{
				hash_table = nc_search_manager(&buffer,bloco1,bloco2,blocos);
			}
		}
	}
	
	return hash_table;
}


GHashTable* auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,Params set){
	
	int n;//Elementos por sequência
	GHashTable* hash_table;
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
	get_setup(&n);
	
	setup_without_cuda(c);
	
	printString("Dados inicializados.\n",NULL);
	printSet(n);
	printString("Iniciando iterações:\n",NULL);
	
    //cudaEventRecord(start,0);
	hash_table = NONcudaIteracoes(bloco1,bloco2,blocos,n);
    //cudaEventRecord(stop,0);
    //cudaEventSynchronize(stop);
    //cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	
return hash_table;	
}

