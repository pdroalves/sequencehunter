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

void setup_for_cuda(char*,vgrafo*,vgrafo*,vgrafo*, vgrafo*);

void aux(int CUDA,char *c){
	
	int m;//Quantidade sequências
	int n;//Elementos por sequência
	int buffer_size = 2;
	Buffer b;
	char *tmp;
	vgrafo *d_a;
	vgrafo *d_c;
	vgrafo *d_g;
	vgrafo *d_t;
	
	cudaMalloc((void**)&d_a,sizeof(vgrafo));
    cudaMalloc((void**)&d_c,sizeof(vgrafo));
    cudaMalloc((void**)&d_g,sizeof(vgrafo));
    cudaMalloc((void**)&d_t,sizeof(vgrafo));
	
	//Inicializa
	get_setup(&m,&n);
	prepare_buffer(&b,buffer_size);
	
	setup_for_cuda(c,d_a,d_c,d_g,d_t);
	
	while( check_file_end()== 0){
		//Realiza loop enquanto existirem sequências para encher o buffer
		fill_buffer(&b,n);
		 tmp = get_antisenso(b.seq[0]);
	}
	
	cudaFree(d_a);
	cudaFree(d_c);
	cudaFree(d_g);
	cudaFree(d_t);
	
	return;

}
void setup_for_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t){
	//Recebe um vetor de caracteres com o padrão a ser procurado
	//Recebe ponteiros para os quatro vértices do grafo já na memória da GPU
	char *d_senso;
	char *d_antisenso;
	int size = strlen(seq);
	
	//Aloca memória na GPU
    cudaMalloc((void**)&d_senso,size*sizeof(char));
    cudaMalloc((void**)&d_antisenso,size*sizeof(char));
    
    //cudaMemcpy(d_seq,seq,size*sizeof(char),cudaMemcpyHostToDevice);
    //cudaMemcpy(d_seq_antisenso,get_antisenso(seq),size*sizeof(char),cudaMemcpyHostToDevice);
    
    //Configura grafos direto na memória da GPU
	//set_grafo<<<1,1>>>(d_seq,d_seq_antisenso,d_a,d_c,d_g,d_t);
	
	cudaFree(d_senso);
	cudaFree(d_antisenso);
	return;
}


