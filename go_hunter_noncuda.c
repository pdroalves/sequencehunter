#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
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

const char tmp_ncuda_s_name[11] = "tmp_sensos";
const char tmp_ncuda_as_name[15] = "tmp_antisensos";
void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,gboolean verb,gboolean sil){
	
	int n;//Elementos por sequência
	vgrafo g_a;
	vgrafo g_c;
	vgrafo g_g;
	vgrafo g_t;
	verbose = verb;
	silent = sil;
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
		  
		  tmp_sensos = fopen(tmp_ncuda_s_name,"w");
		  tmp_antisensos = fopen(tmp_ncuda_as_name,"w");
		  
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





