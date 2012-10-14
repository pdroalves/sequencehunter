//      aux.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include "cuda.h"
#include "cuda_runtime_api.h"
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "log.h"
#include "fila.h"

#define buffer_size 4 // Capacidade máxima do buffer
#define FILA_MIN 10000 // Tamanho minimo da fila antes de começar a esvazia-la
const char tmp_s_name[11] = "tmp_sensos";
const char tmp_as_name[15] = "tmp_antisensos";
__constant__ char *d_buffer[buffer_size];
omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
char **data;

void auxCUDA(char *c,const int bloco1,const int bloco2,const int blocos);
void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos);
void setup_for_cuda(char*,vgrafo*,vgrafo*,vgrafo*, vgrafo*);
void setup_without_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t);
void load_buffer_CUDA(Buffer *d_buffer,int *load,int n);
void load_buffer_NONCuda(Buffer *b,int n);
void cudaIteracoes(int bloco1,int bloco2,int blocoV,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t);
void NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t);
	
void aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos,gboolean disable_cuda,gboolean sil,gboolean verb){
	verbose = verb;
	sil = silent;
	if(CUDA)
		auxCUDA(c,bloco1,bloco2,blocos);
	else
		auxNONcuda(c,bloco1,bloco2,blocos);
return;
}

void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos){
	
	int n;//Elementos por sequência
	vgrafo g_a;
	vgrafo g_c;
	vgrafo g_g;
	vgrafo g_t;
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
	NONcudaIteracoes(bloco1,bloco2,blocos,n,&g_a,&g_c,&g_g,&g_t);
    //cudaEventRecord(stop,0);
    //cudaEventSynchronize(stop);
    //cudaEventElapsedTime(&tempo,start,stop);
    
	printString("Iterações terminadas. Tempo: ",NULL);
	print_tempo(tempo);
	destroy_grafo(&g_a,&g_c,&g_g,&g_t);
	
return;	
}
	
void auxCUDA(char *c,const int bloco1,const int bloco2,const int blocos){
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

void load_buffer_CUDA(Buffer *d_buffer,int *load,int n){
	int i;
	Buffer h_buffer;
	
	if(*load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregadosb->capacidade = c;
		h_buffer.seq = (char**)malloc(buffer_size*sizeof(char*));
		h_buffer.capacidade = buffer_size;
		h_buffer.load = 0;
		for(i=0; i < buffer_size;i++) h_buffer.seq[i] = (char*)malloc((n+1)*sizeof(char));
		
		fill_buffer(&h_buffer,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
		*load = h_buffer.load;
		if(*load != -1){
			print_seqs_carregadas(*load);
			//printf("%s\n",b->seq[0]);
			d_buffer->load = *load;
			//Copia sequencias para GPU e grava os enderecos em d_buffer->seq
			for(i=0;i<(*load);i++)
				cudaMemcpy(d_buffer->seq[i],h_buffer.seq[i],(n+1)*sizeof(char),cudaMemcpyHostToDevice);
			//Copia os enderecos das sequencias para a GPU
			cudaMemcpy(data,d_buffer->seq,(*load)*sizeof(char*),cudaMemcpyHostToDevice);	
		}
			
	}
		
	//Destroy h_buffer
	//for(i=0;i<buffer_size;i++) free(h_buffer.seq);
	return;
}


void load_buffer_NONCuda(Buffer *b,int n){
	
	if(b->load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
		fill_buffer(b,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
		if(b->load != -1){
			//print_seqs_carregadas(b->load);
			//printf("%s\n",b->seq[0]);	
		}
			
	}
		
	
	return;
}

void NONcudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t){
	
	Buffer buffer;
	char *tmp;
	int blocoV = blocos - bloco1 - bloco2+1;
	int buffer_size_NC = 5120;
	int i;
	int tam;
	int p=0;
	Fila *f_sensos;
	Fila *f_antisensos;
	
	//Inicializa buffer
	prepare_buffer(&buffer,buffer_size_NC);
	f_sensos = criar_fila();
	f_antisensos = criar_fila();
	start_fila_lock();
			
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
		  FILE *tmp_sensos;
		  FILE *tmp_antisensos;
		  
		  tmp_sensos = fopen(tmp_s_name,"w");
		  tmp_antisensos = fopen(tmp_as_name,"w");
		  
		  while( buffer.load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			

		  while(buffer.load != -1){
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
		#pragma omp section
		{
		//////////////////////////////////////////
		// Realiza as iteracoes///////////////////
		//////////////////////////////////////////
		
		int *resultados;
		resultados = (int*)malloc(buffer_size_NC*sizeof(int));
		
			while( buffer.load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
			while(buffer.load != -1){
				//Realiza loop enquanto existirem sequências para encher o buffer
				
					busca(bloco1,bloco2,blocos,&buffer,resultados,d_a,d_c,d_g,d_t);//Kernel de busca
					
					tam = buffer.load;
					p += tam;
					//printf("%d\n",p);
					for(i = 0; i < tam;i++){//Copia sequências senso e antisenso encontradas
						switch(resultados[i]){
							case 1:
								tmp = buffer.seq[i];
								if(verbose == TRUE && silent != TRUE)	
									printf("S: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_sensos));
								enfileirar(f_sensos,tmp);
								//printString("Senso:",tmp);
								buffer.load--;
							break;
							case 2:
								tmp = buffer.seq[i];
								if(verbose == TRUE && silent != TRUE)
									printf("N: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_antisensos));
								enfileirar(f_antisensos,get_antisenso(tmp));
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
		//////////////////////////////////////////
		//////////////////////////////////////////
		//////////////////////////////////////////
		}
		
	}
}
	
	//printf("Iterações executadas: %d.\n",iter);
	//free(tmp);
	return;
}


void cudaIteracoes(int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t){
	
	Buffer buffer;
	int blocoV = blocos - bloco1 - bloco2+1;
	int i;
	int tam;
	int p=0;
	int buffer_load;
	Fila *f_sensos;
	Fila *f_antisensos;
	
	//Inicializa buffer
	prepare_buffer_CUDA(&buffer,n,buffer_size);
	buffer_load = 0;
	f_sensos = criar_fila();
	f_antisensos = criar_fila();
	start_fila_lock();
	cudaMalloc((void**)&data,buffer_size*sizeof(char*));
			
	#pragma omp parallel num_threads(3) shared(buffer) shared(f_sensos) shared(f_antisensos) shared(buffer_load)
	{	
		
		#pragma omp sections
		{
		#pragma omp section
		{
		//////////////////////////////////////////
		// Carrega o buffer //////////////////////
		//////////////////////////////////////////
			while(buffer_load != -1){//Looping até o final do buffer
				//printf("%d.\n",buffer.load);
				if(buffer_load == 0)
					load_buffer_CUDA(&buffer,&buffer_load,n);
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
		  FILE *tmp_sensos;
		  FILE *tmp_antisensos;
		  
		  tmp_sensos = fopen(tmp_s_name,"w");
		  tmp_antisensos = fopen(tmp_as_name,"w");
		  
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
		#pragma omp section
		{
			//////////////////////////////////////////
			// Realiza as iteracoes///////////////////
			//////////////////////////////////////////
			int *hold;
			char **resultados;
			int num_blocks;
			int num_threads;
			num_blocks = 1;
			num_threads = buffer_size;
			char *tmp;
			
			tmp = (char*)malloc((blocoV+1)*sizeof(char));
			hold = (int*)malloc(buffer_size*sizeof(int));
			cudaMalloc((void**)&resultados,buffer_size*sizeof(char*));
			
			while( buffer_load == 0){
			}//Aguarda para que o buffer seja enchido pela primeira vez
			
			while(buffer_load != -1){
				//Realiza loop enquanto existirem sequências para encher o buffer
					printf("///////////////////////\n");
					k_busca(num_blocks,num_threads,bloco1,bloco2,blocos,data,resultados,d_a,d_c,d_g,d_t);//Kernel de busca
					tam = buffer_load;
					p += tam;
					//printf("%d\n",p);
					cudaMemcpy(hold,resultados,buffer_size*sizeof(int),cudaMemcpyDeviceToHost);
					checkCudaError();
					for(i = 0; i < tam;i++){//Copia sequências senso e antisenso encontradas
						switch(hold[i]){
							case SENSO:
								cudaMemcpy(tmp,buffer.seq[i],(blocoV+1)*sizeof(char),cudaMemcpyDeviceToHost);
								checkCudaError();
								if(verbose == TRUE && silent != TRUE)	
									printf("S: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_sensos));
								enfileirar(f_sensos,tmp);
								//printString("Senso:",tmp);
								buffer_load--;
							break;
							case ANTISENSO:
								cudaMemcpy(tmp,buffer.seq[i],(blocoV+1)*sizeof(char),cudaMemcpyDeviceToHost);
								checkCudaError();
								if(verbose == TRUE && silent != TRUE)
									printf("N: %s - %d - F: %d\n",tmp,p,tamanho_da_fila(f_antisensos));
								enfileirar(f_antisensos,get_antisenso(tmp));
								//printString("Antisenso:",tmp);
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
			//////////////////////////////////////////
			//////////////////////////////////////////
			//////////////////////////////////////////
			cudaFree(resultados);
			}
		
		}
	}
	
	//printf("Iterações executadas: %d.\n",iter);
	//free(tmp);
	cudaDeviceReset();
	cudaFree(data);
	return;
}


