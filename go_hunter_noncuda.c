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

omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
omp_lock_t MC_copy_lock;

const char tmp_ncuda_s_name[11] = "tmp_sensos";
const char tmp_ncuda_as_name[15] = "tmp_antisensos";


void setup_without_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t){
	//Recebe um vetor de caracteres com o padrão a ser procurado
	//Recebe ponteiros para os quatro vértices do grafo

    //Configura grafo
    char* hold = get_antisenso(seq);
	set_grafo_NONCuda(seq,hold,d_a,d_c,d_g,d_t);
	printString("Grafo de busca contigurado.",NULL);
	free(hold);
	return;
}




void load_buffer_NONCuda(Buffer *b,int n){
	
	if(b->load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
		fill_buffer(b);//Enche o buffer e guarda a quantidade de sequências carregadas.			
	}
		
	
	return;
}

GHashTable* NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t){
	
	Buffer buffer;
	char *tmp;
	int blocoV = blocos - bloco1 - bloco2+1;
	int buffer_size_NC = buffer_size;
	int i;
	int tam;
	int p=0;
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
		//////////////////////////////////////////
		// Carrega o buffer //////////////////////
		//////////////////////////////////////////
			while(buffer.load != -1){//Looping até o final do buffer
				//printf("%d.\n",buffer.load);
				if(buffer.load == 0)
					load_buffer_NONCuda(&buffer,n);
			}
		
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////	
		}
		#pragma omp section
		{
		//////////////////////////////////////////
		// Libera memoria ////////////////////////
		//////////////////////////////////////////
		  int retorno;
		  char *hold;
			hash_table = criar_ghash_table();
		  while( buffer.load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			

	
					  while(buffer.load != GATHERING_DONE){
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
				
					
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
		}
		#pragma omp section
		{
		//////////////////////////////////////////
		// Realiza as iteracoes///////////////////
		//////////////////////////////////////////
		
		int *resultados;
		cudaEvent_t startK,stopK,start,stop;
		float elapsedTimeK,elapsedTime;
		float iteration_time;
		resultados = (int*)malloc(buffer_size_NC*sizeof(int));
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventCreate(&startK);
		cudaEventCreate(&stopK);
			FILE *busca_,*retorno;
				
				busca_ = fopen("noncuda_busca.dat","w+");
				retorno = fopen("noncuda_retorno.dat","w+");
		iteration_time = 0;
		
			while( buffer.load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
				cudaEventRecord(start,0);
			while(buffer.load != GATHERING_DONE){
				//Realiza loop enquanto existirem sequências para encher o buffer
				cudaEventRecord(stop,0);
				cudaEventSynchronize(stop);
				cudaEventElapsedTime(&elapsedTime,start,stop);
				iteration_time += elapsedTime;
				printf("Tempo até retornar busca em %.2f ms\n",elapsedTime);
				//fprintf(retorno,"%f\n",elapsedTime);
					
				cudaEventRecord(startK,0);
					busca(bloco1,bloco2,blocos,&buffer,resultados,d_a,d_c,d_g,d_t);//Kernel de busca
					
				cudaEventRecord(stopK,0);
				cudaEventSynchronize(stopK);
				cudaEventElapsedTime(&elapsedTimeK,startK,stopK);
				iteration_time += elapsedTimeK;
				printf("Execucao da busca em %.2f ms\n",elapsedTimeK);
				//fprintf(busca_,"%f\n",elapsedTimeK);
				cudaEventRecord(start,0);
						
					
					tam = buffer.load;
					p += tam;
					/*	if(p > 1000000) {
							
							fclose(busca_);
							fclose(retorno);
							exit(0);
						}*/
							
					//printf("%d\n",p);
					for(i = 0; i < tam;i++){//Copia sequências senso e antisenso encontradas
						switch(resultados[i]){
							case 1:
								tmp = buffer.seq[i];
								//if(verbose == TRUE && silent != TRUE)	
								//	printf("S: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_sensos));
								omp_set_lock(&MC_copy_lock);
								enfileirar(f_sensos,tmp);
								omp_unset_lock(&MC_copy_lock);
								
								//printString("Senso:",tmp);
								buffer.load--;
							break;
							case 2:
								tmp = buffer.seq[i];
								//if(verbose == TRUE && silent != TRUE)
								//	printf("N: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_antisensos));
								omp_set_lock(&MC_copy_lock);	
								enfileirar(f_antisensos,get_antisenso(tmp));
								omp_unset_lock(&MC_copy_lock);
								
								//printString("Antisenso:",tmp);
								buffer.load--;
							break;
							default:
								buffer.load--;
							break;
						}
					}
					
										
					if(buffer.load != 0)
					{
						printf("Erro! Buffer não foi totalmente esvaziado.\n");
						buffer.load = 0;
					}
					
					while(buffer.load==0){}
									
			}
				//fclose(busca_);
				//fclose(retorno);
				
				printf("Busca realizada em %.2f ms.\n",iteration_time);
	
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
		}
		
	}
}
	
	return hash_table;
}



GHashTable* auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,gboolean verb,gboolean sil){
	
	int n;//Elementos por sequência
	vgrafo g_a;
	vgrafo g_c;
	vgrafo g_g;
	vgrafo g_t;
	verbose = verb;
	silent = sil;
	GHashTable* hash_table;
	//Arrumar nova maneira de contar o tempo sem usar a cuda.h
	//cudaEvent_t start;
	//cudaEvent_t stop;
	//cudaEventCreate(&start);
	//cudaEventCreate(&stop);
	float tempo = 0;
	printf("OpenMP Mode.\n");
	get_setup(&n);
	
	setup_without_cuda(c,&g_a,&g_c,&g_g,&g_t);
	
	printString("Dados inicializados.\n",NULL);
	printSet(n);
	printString("Iniciando iterações:\n",NULL);
	
    //cudaEventRecord(start,0);
	hash_table = NONcudaIteracoes(bloco1,bloco2,blocos,n,&g_a,&g_c,&g_g,&g_t);
    //cudaEventRecord(stop,0);
    //cudaEventSynchronize(stop);
    //cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	destroy_grafo(&g_a,&g_c,&g_g,&g_t);
	
return hash_table;	
}

