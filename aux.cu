//      aux.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include <cuda.h>
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include <stdio.h>

#define buffer_size 1
__constant__ char *d_buffer[buffer_size];

void setup_for_cuda(char*,vgrafo*,vgrafo*,vgrafo*, vgrafo*);

void aux(int CUDA,char *c){
	
	int m;//Quantidade sequências
	int n;//Elementos por sequência
	int *matchs;
	int *d_matchs;
	int i;
	Buffer buffer;
	char **tmp;
	char *check;
	vgrafo *d_a;
	vgrafo *d_c;
	vgrafo *d_g;
	vgrafo *d_t;
	char **s;
	
	get_setup(&m,&n);
	
	check = (char*)malloc((n+1)*sizeof(char));
	cudaMalloc((void**)&d_a,sizeof(vgrafo));
    cudaMalloc((void**)&d_c,sizeof(vgrafo));
    cudaMalloc((void**)&d_g,sizeof(vgrafo));
    cudaMalloc((void**)&d_t,sizeof(vgrafo));
    
    cudaMalloc((void**)&d_matchs,buffer_size*sizeof(int));
    cudaMalloc((void**)&s,buffer_size*sizeof(char*));
	matchs = (int*)calloc(0,buffer_size*sizeof(int));
	cudaMemcpy(d_matchs,matchs,buffer_size*sizeof(int),cudaMemcpyHostToDevice);
		
	//Inicializa
	prepare_buffer(&buffer,buffer_size);
	setup_for_cuda(c,d_a,d_c,d_g,d_t);
	
	while( check_file_end()== 0){
		//Realiza loop enquanto existirem sequências para encher o buffer
		fill_buffer(&buffer,n);
		for(i=0;i<buffer_size;i++){
			cudaMalloc((void**)&d_buffer[i],(n+1)*sizeof(char));
			cudaMemcpy(d_buffer[i],buffer.seq[i],(n+1)*sizeof(char),cudaMemcpyHostToDevice);
		}
		cudaMemcpy(s,d_buffer,buffer_size*sizeof(char*),cudaMemcpyHostToDevice);
		k_busca<<<1,buffer_size>>>(d_matchs,s,strlen(c),d_a,d_c,d_g,d_t);
		//k_teste<<<1,buffer_size>>>(d_matchs,d_buffer,n,d_a,d_c,d_g,d_t);
		//k_printf<<<1,1>>>(s);
		
		cudaMemcpy(matchs,d_matchs,buffer_size*sizeof(int),cudaMemcpyDeviceToHost);
		for(i=0;i<buffer_size;i++)
			printf("%s:-> %d\n",buffer.seq[i],matchs[i]);
	}
	
	cudaFree(d_a);
	cudaFree(d_c);
	cudaFree(d_g);
	cudaFree(d_t);
	cudaFree(d_matchs);
	free(matchs);
	
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
	printf("Grafo contigurado\n");
	cudaFree(d_senso);
	cudaFree(d_antisenso);
	return;
}


