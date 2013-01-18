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

#define OMP_NTHREADS 3
#define THREAD_BUFFER_LOADER 0
#define THREAD_SEARCH 1
#define THREAD_CLEANER 2

gboolean THREAD_DONE[3];
omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
gboolean debug;
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

GHashTable* nc_memory_cleaner_manager(Buffer *buffer,Fila *f_sensos,Fila *f_antisensos){
	//////////////////////////////////////////
		// Libera memoria ////////////////////////
		//////////////////////////////////////////
		THREAD_DONE[THREAD_CLEANER] = FALSE;
		  int retorno;
		  char *hold;
		  GHashTable *hash_table = criar_ghash_table();
		  while( buffer->load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
					  while(buffer->load != GATHERING_DONE || 
							THREAD_DONE[THREAD_SEARCH] == FALSE){
								
						if(tamanho_da_fila(f_sensos) > 0){
							omp_set_lock(&MC_copy_lock);
							hold = desenfileirar(f_sensos);
							omp_unset_lock(&MC_copy_lock);
								
								
							retorno = adicionar_ht(hash_table,hold,criar_value(0,1,0,0));
						}
						if(tamanho_da_fila(f_antisensos) > 0){
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
		return hash_table;			
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
}

void nc_search_manager(Buffer *buffer,int bloco1,int bloco2,int blocos,Fila *f_sensos,Fila *f_antisensos){
	//////////////////////////////////////////
		// Realiza as iteracoes///////////////////
		//////////////////////////////////////////
		
		THREAD_DONE[THREAD_SEARCH] = FALSE;
		int *resultados;
		int tam;
		int i;
		int p=0;
		char *tmp;
		cudaEvent_t startK,stopK,start,stop;
		float elapsedTimeK,elapsedTime;
		float iteration_time;
		int fsensos,fasensos;
		FILE *c;
		c = fopen("ncuda","w");
		fsensos=fasensos=0;
		resultados = (int*)malloc(buffer_size_NC*sizeof(int));
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
				if(debug){
					printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
					printString("Retorno da busca:\n",NULL);
					print_tempo_optional(elapsedTime);
				}
				cudaEventRecord(startK,0);
					busca(bloco1,bloco2,blocos,buffer,resultados);//Kernel de busca
					
				cudaEventRecord(stopK,0);
				cudaEventSynchronize(stopK);
				cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
				iteration_time += elapsedTimeK;
				if(debug){
					printf("Execucao da busca em %.2f ms\n",elapsedTimeK);
					printString("Execucao da busca:\n",NULL);
					print_tempo_optional(elapsedTimeK);
				}
				cudaEventRecord(start,0);
						
					
					tam = buffer->load;
					p += tam;
					
				for(i = 0; i < tam;i++){//Copia sequências senso e antisenso encontradas
						switch(resultados[i]){
							case SENSO:
								tmp = buffer->seq[i];
								//if(verbose == TRUE && silent != TRUE)	
								//	printf("S: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_sensos));
							 //	if(debug)
								//	fprintf(c,"%s\n",tmp);
								fsensos++;
								omp_set_lock(&MC_copy_lock);
								enfileirar(f_sensos,tmp);
								omp_unset_lock(&MC_copy_lock);
								
								//printString("Senso:",tmp);
								buffer->load--;
							break;
							case ANTISENSO:
								tmp = buffer->seq[i];
								//if(verbose == TRUE && silent != TRUE)
								//	printf("N: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_antisensos));
								fasensos++;
								omp_set_lock(&MC_copy_lock);	
								enfileirar(f_antisensos,get_antisenso(tmp));
								omp_unset_lock(&MC_copy_lock);
								
								//printString("Antisenso:",tmp);
								buffer->load--;
							break;
							default:
								buffer->load--;
							break;
						}
					}
					
					if(verbose && !silent)		
						printf("Sequencias processadas: %d - S: %d, AS: %d\n",p,fsensos,fasensos);
					if(debug){
							printf("Filas - S: %d, AS: %d\n\n",tamanho_da_fila(f_sensos),tamanho_da_fila(f_antisensos));
							print_seqs_filas_optional(tamanho_da_fila(f_sensos),tamanho_da_fila(f_antisensos));
					}		
					
					if(buffer->load != 0)
					{
						if(debug)
							printf("Erro! Buffer não foi totalmente esvaziado.\n");
						if(buffer->load != -1)
							buffer->load = 0;
					}
					
					while(buffer->load==0){}
									
			}
				
			
				if(iteration_time > 10000)
					printf("Busca realizada em %.2f s.\n",iteration_time/(float)60000);
				else 
					printf("Busca realizada em %.2f ms.\n",iteration_time);
	
	
	
		THREAD_DONE[THREAD_SEARCH] = TRUE;
		return;
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
		
}



GHashTable* NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n){
	
	Buffer buffer;
	int blocoV = blocos - bloco1 - bloco2+1;
	Fila *f_sensos;
	Fila *f_antisensos;
	GHashTable* hash_table;
	
	//Inicializa buffer
	prepare_buffer(&buffer,buffer_size_NC);
	f_sensos = criar_fila();
	f_antisensos = criar_fila();
	omp_init_lock(&MC_copy_lock);
	
			
	#pragma omp parallel num_threads(3) shared(buffer) shared(f_sensos) shared(f_antisensos)
	{	
		
		#pragma omp sections
		{
			#pragma omp section
			{
				nc_buffer_manager(&buffer,n);
			}
			#pragma omp section
			{
				hash_table = nc_memory_cleaner_manager(&buffer,f_sensos,f_antisensos);
			}
			#pragma omp section
			{
				nc_search_manager(&buffer,bloco1,bloco2,blocos,f_sensos,f_antisensos);
			}
		}
	}
	
	return hash_table;
}



GHashTable* auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,gboolean verb,gboolean sil,gboolean deb){
	
	int n;//Elementos por sequência
	verbose = verb;
	silent = sil;
	debug = deb;
	GHashTable* hash_table;
	//Arrumar nova maneira de contar o tempo sem usar a cuda.h
	//cudaEvent_t start;
	//cudaEvent_t stop;
	//cudaEventCreate(&start);
	//cudaEventCreate(&stop);
	float tempo = 0;
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

