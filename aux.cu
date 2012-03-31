//      aux.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include <stdio.h>
#include <cuda.h>
#include <omp.h> 
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "cuda_stack.h"
#include "log.h"
#include "pilha.h"

#define buffer_size 10//Capacidade máxima do buffer
__constant__ char *d_buffer[buffer_size];
int buffer_flag = 1;//0 se o buffer já foi carregado, 1 se estiver sendo carregado.

void setup_for_cuda(char*,vgrafo*,vgrafo*,vgrafo*, vgrafo*);
void load_buffer(Buffer *b,char** s,int n);

void aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos){
	
	int m;//Quantidade sequências
	int n;//Elementos por sequência
	int *matchs;
	int *d_matchs;
	int i;
	int blocoV;
	Buffer buffer;
	vgrafo *d_a;
	vgrafo *d_c;
	vgrafo *d_g;
	vgrafo *d_t;
	char **s;
	char **d_sensos;
	char **d_antisensos;
	char **d_sensos_hold;
	char **d_antisensos_hold;
	char *tmp;
  cudaEvent_t start;
  cudaEvent_t stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);
	pilha p_sensos;
	pilha p_antisensos;
	int p_sensos_size;
	int p_antisensos_size;
	
	get_setup(&m,&n);
	blocoV = blocos - bloco1 - bloco2+1;
	cudaMalloc((void**)&d_a,sizeof(vgrafo));
    cudaMalloc((void**)&d_c,sizeof(vgrafo));
    cudaMalloc((void**)&d_g,sizeof(vgrafo));
    cudaMalloc((void**)&d_t,sizeof(vgrafo));
    
    cudaMalloc((void**)&s,buffer_size*sizeof(char*));
    
	cudaMalloc((void**)&d_sensos,buffer_size*sizeof(char*));
	cudaMalloc((void**)&d_antisensos,buffer_size*sizeof(char*));
	tmp = (char*)malloc(blocoV*sizeof(char));
	d_sensos_hold = (char**)malloc(buffer_size*sizeof(char*));
	d_antisensos_hold = (char**)malloc(buffer_size*sizeof(char*));
	for(i=0;i<buffer_size;i++){
		cudaMalloc((void**)&d_sensos_hold[i],blocoV*sizeof(char));//Aloco n+1 posicoes para as bases e +1 para processamento interno no arquivo cuda_stack.cu
		cudaMalloc((void**)&d_antisensos_hold[i],blocoV*sizeof(char));//Aloco n+1 posicoes para as bases e +1 para processamento interno no arquivo cuda_stack.cu
	}
	cudaMemcpy(d_sensos,d_sensos_hold,buffer_size*sizeof(char*),cudaMemcpyHostToDevice);
	cudaMemcpy(d_antisensos,d_antisensos_hold,buffer_size*sizeof(char*),cudaMemcpyHostToDevice);
	
	
	cudaMalloc((void**)&d_matchs,buffer_size*sizeof(int));
	matchs = (int*)calloc(0,buffer_size*sizeof(int));
	cudaMemcpy(d_matchs,matchs,buffer_size*sizeof(int),cudaMemcpyHostToDevice);
	
	p_sensos = criar_pilha();
	p_antisensos = criar_pilha();
		
	//Inicializa
	prepare_buffer(&buffer,buffer_size);
	setup_for_cuda(c,d_a,d_c,d_g,d_t);
	
	printString("Dados inicializados.\n",NULL);
	printSet(m,n);
	printString("Iniciando iterações:\n",NULL);
	
	#pragma omp parallel sections num_threads(2) shared(buffer)
	{
	#pragma omp section
	{	
	load_buffer(&buffer,s,n);
	printf("Buffer vazio.\n");
	}
	
	#pragma omp section
	{
	while( buffer.load != -1){
		//Realiza loop enquanto existirem sequências para encher o buffer
		while(buffer_flag != 0){
		}//Espera o buffer ser carregado
		if(buffer.load != -1){
		dim3 dimBlock(buffer_size>=buffer.load?buffer.load:buffer_size);
		dim3 dimGrid(1);
		k_busca<<<dimGrid,dimBlock>>>(bloco1,bloco2,blocos,s,d_a,d_c,d_g,d_t);
		
		k_recupera<<<dimGrid,dimBlock>>>(s,d_sensos,d_antisensos);
		for(i=0;i<buffer_size;i++){//Copia sequências senso e antisenso encontradas
			cudaMemcpy(tmp,d_sensos_hold[i],sizeof(char),cudaMemcpyDeviceToHost);
			if(tmp[0] != '\0'){//Antes de copiar, verifica se existe alguma sequência útil
				cudaMemcpy(tmp,d_buffer[i],blocoV*sizeof(char),cudaMemcpyDeviceToHost);
				//printf("%s\n",tmp);
				empilha(tmp,&p_sensos);
			}
			
			cudaMemcpy(tmp,d_antisensos_hold[i],sizeof(char),cudaMemcpyDeviceToHost);
			if(tmp[0] != '\0'){//Antes de copiar, verifica se existe alguma sequência útil
				cudaMemcpy(tmp,d_buffer[i],blocoV*sizeof(char),cudaMemcpyDeviceToHost);
				//printf("%s\n",tmp);
				empilha(tmp,&p_antisensos);
			}
		}
		buffer.load = 0;//Avisando sobre buffer vazio
	}
	}
	
	
	}
	
	}
	p_sensos_size = tamanho_da_pilha(&p_sensos);
	p_antisensos_size = tamanho_da_pilha(&p_antisensos);
	printf("Sensos: %d.\nAntisensos: %d.\n",p_sensos_size,p_antisensos_size);
	
	for(i=0;i<p_sensos_size;i++){
		printf("%s\n",desempilha(&p_sensos));
	}
	
	printString("Iterações terminadas.",NULL);
	print_matchs(p_sensos_size,p_antisensos_size);
	
	cudaFree(d_a);
	cudaFree(d_c);
	cudaFree(d_g);
	cudaFree(d_t);
	cudaFree(d_matchs);
	free(matchs);
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
    cudaMemcpy(d_antisenso,get_antisenso(seq),size*sizeof(char),cudaMemcpyHostToDevice);
    
    //Configura grafos direto na memória da GPU
	set_grafo<<<1,1>>>(d_senso,d_antisenso,d_a,d_c,d_g,d_t);
	printString("Grafo de busca contigurado.",NULL);
	cudaFree(d_senso);
	cudaFree(d_antisenso);
	return;
}

void load_buffer(Buffer *b,char** s,int n){
	//Essa função fica em loop até que todas as sequências tenham sido lidas e processadas
	int i;
	
	fill_buffer(b,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
	print_seqs_carregadas(b->load);
	
	for(i=0;i<buffer_size;i++)
			cudaMalloc((void**)&d_buffer[i],(n+2)*sizeof(char));//Aloco n+1 posicoes para as bases e +1 para processamento interno no arquivo cuda_stack.cu
	
	while(b->load != -1){
		///////////////////////////////////
		buffer_flag = 1;//Sinal fechado////
		///////////////////////////////////
		fill_buffer(b,buffer_size);//Enche o buffer e guarda a quantidade de sequências carregadas.
		if(b->load != -1){
		print_seqs_carregadas(b->load);
		
		for(i=0;i<buffer_size;i++){
			cudaMemcpy(d_buffer[i],b->seq[i],(n+1)*sizeof(char),cudaMemcpyHostToDevice);
		}
		cudaMemcpy(s,d_buffer,buffer_size*sizeof(char*),cudaMemcpyHostToDevice);
		
		//////////////////////////////////
		buffer_flag = 0;//Sinal aberto////
		//////////////////////////////////
		
		while(b->load > 0){
		}
		}else{
			//////////////////////////////////
			buffer_flag = 0;//Sinal aberto////
			//////////////////////////////////
			printf("Não há mais sequências.\n");
			return;
		}
		
	}
	return;
}


