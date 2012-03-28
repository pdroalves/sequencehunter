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

void setup_for_cuda(char*);

void aux(int CUDA){
	
	int m;//Quantidade sequências
	int n;//Elementos por sequência
	int buffer_size = 2;
	Buffer b;
	char *tmp;
	vgrafo *d_a;
	vgrafo *d_c;
	vgrafo *d_g;
	vgrafo *d_t;
	
	//Inicializa
	get_setup(&m,&n);
	prepare_buffer(&b,buffer_size);
	
	while( check_file_end()== 0){
		//Realiza loop enquanto existirem sequências para encher o buffer
		fill_buffer(&b,n);
		 tmp = get_antisenso(b.seq[0]);
	}

}
void setup_for_cuda(char *seq,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g, vgrafo *d_t){
	
	char *d_seq;
	char *d_seq_antisenso;
	
	//Aloca memória na GPU
	cudaMalloc((void**)&d_a,sizeof(vgrafo));
    cudaMalloc((void**)&d_c,sizeof(vgrafo));
    cudaMalloc((void**)&d_g,sizeof(vgrafo));
    cudaMalloc((void**)&d_t,sizeof(vgrafo));
    cudaMalloc((void**)&d_seq,conta_posicoes(seq)*sizeof(char));
    cudaMalloc((void**)&d_seq_antisenso,conta_posicoes(seq)*sizeof(char));
    
    cudaMemcpy(d_seq,seq,conta_posicoes(seq)*sizeof(char),cudaMemcpyHostToDevice);
    cudaMemcpy(d_seq_antisenso,get_antisenso(seq),conta_posicoes(seq)*sizeof(char),cudaMemcpyHostToDevice);
    
    //Configura grafos direto na memória da GPU
    build_grafo<<<1,1>>>(&d_a,&d_c,&d_g,&d_t);
	set_grafo<<<1,1>>>(d_seq,d_seq_antisenso,&d_a,&d_c,&d_g,&d_t);
		
	return;
}


