//      busca.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação do algoritmo de busca.
//
//		27/03/2012

#include <stdio.h>
#include <string.h>
#include <cuda.h>
#include "../Headers/estruturas.h"
#include "../Headers/cuda_functions.h"
#include "../Headers/log.h"

#define ABSOLUTO(a) a>=0?a:-a

__constant__ short int d_matrix_senso[MAX_SEQ_SIZE];
__constant__ short int d_matrix_antisenso[MAX_SEQ_SIZE];

extern "C" void cuda_convert_to_graph(char *s,short int s_len,short int *vector){
  int i;
  for(i=0;i<s_len-1;i++)
    vector[i] = s[i]*(2+s[i+1]);
  return;
}

void convert_target_to_graph(char *s,int s_len,short int *vector){
  int i;
  for(i=0;i<s_len-1;i++)
    if(s[i] == 'N' || s[i+1] == 'N')
      vector[i] = -1*s[i]*(2+s[i+1]);
    else
      vector[i] = s[i]*(2+s[i+1]);
  vector[s_len-1] = 0;
  return;
}

__device__ int get_candidate_table(short int start_vertex,short int *vertexes,short int v_size,short int *table){
  int i;
  int j = 0;
  for(i=0;i<v_size;i++){
    if(vertexes[i] == start_vertex){
      table[j] = i;
      j++;
    }
  }
  return j;
}

__device__ int match_check(short int *target,short int target_size,short int *analyse){
  int i;
  for(i=0;i<target_size;i++){
    if(target[i] > 0)
      if(target[i] != analyse[i])
        return 0;
  }
  return 1;
}

extern "C" void checkCudaError();

////////////////////////////////////////////////////////////////////////////////////////
//////////////////					Buscador		 				////////////////////
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
///////////////				Metodo de busca com CUDA				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

__global__ void k_buscador(int totalseqs,
										int seqSize_an,
										short int *vertexes,
										short int *candidates,
										short int *resultados,
										short int *search_gaps,
										int bloco1,
										int bloco2,
										int blocoV){

  ////////		UM THREAD POR SEQUENCIA
  ////////
  ////////
  ////////
  ////////		seqSize_an: o tamanho da sequencia analisada
  ////////		seqSize_busca: o tamanho da sequencia alvo
  ////////
  ////////
  ////////
  
	unsigned int seqId = threadIdx.x + blockIdx.x*blockDim.x;;// id da sequencia analisada
	short int seqSize_bu = bloco1+bloco2+blocoV;;// Tamanho da sequencia alvo
	short tipo;
	short int *this_candidates = &candidates[seqId*seqSize_an];
	short int *this_vertexes = &vertexes[seqId*seqSize_an];
	short int num_sensos_candidates;
	short int num_antisensos_candidates;
	short int candidate_pos_sensos;
	short int candidate_pos_antisensos;
	short int i;

	if(seqId < totalseqs){
		num_sensos_candidates = get_candidate_table(d_matrix_senso[0],this_vertexes,seqSize_an-seqSize_bu+1,this_candidates);
		tipo = 0;
	  	for(i=0;i<num_sensos_candidates && !tipo;i++){
			candidate_pos_sensos = this_candidates[i];
			if(match_check(d_matrix_senso,seqSize_bu,&this_vertexes[candidate_pos_sensos])){
			  search_gaps[seqId] = i + bloco1;
			  tipo = SENSO;  
			}
		}
		if(!tipo){		
			num_antisensos_candidates = get_candidate_table(d_matrix_antisenso[0],this_vertexes,seqSize_an-seqSize_bu+1,this_candidates);
			for(i=0;i<num_antisensos_candidates && !tipo;i++){
			 candidate_pos_antisensos = this_candidates[i];
			  if(match_check(d_matrix_antisenso,seqSize_bu,&this_vertexes[candidate_pos_antisensos])){
			    search_gaps[seqId] = i + bloco2;
			    tipo = ANTISENSO;
			  }
			}
		}		
								 
		resultados[seqId] = tipo;	 
	}
	return;
}

extern "C" void k_busca(const int loaded,const int seqSize_an,const int seqSize_bu,int bloco1,int bloco2,int blocoV,short int *vertexes,short int *candidates,short int *resultados,short int *search_gaps,cudaStream_t stream){
	int num_threads;
	int num_blocks;
	
	if(loaded > MAX_CUDA_THREADS_PER_BLOCK){
		num_threads = MAX_CUDA_THREADS_PER_BLOCK;
		num_blocks = (float)loaded/(float)num_threads + 1;
	}else{
		num_threads = loaded;
		num_blocks = 1;
	}
	
	dim3 dimBlock(num_threads);
	dim3 dimGrid(num_blocks);
	
	k_buscador<<<dimGrid,dimBlock,0,stream>>>(loaded,seqSize_an,vertexes,candidates,resultados,search_gaps,bloco1,bloco2,blocoV);
	
	checkCudaError();
	return;
}
////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////   	Auxiliar     ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

extern "C" void checkCudaError(){
	char erro[100];
	strcpy(erro,cudaGetErrorString(cudaGetLastError()));
    if(strcmp(erro,"no error") != 0){
		printf("%s\n",erro);
    }   
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void getMatrix(short int *matrix,char *str){
	// Matrix já deve vir alocada
	int size_y;
	int i;

	size_y = strlen(str);

	// Preenche matriz
	for(i = 0; i < size_y;i++){
		matrix[i] = str[i];
	}	

	return;
}

 void set_grafo(char *senso,char *antisenso,short int *matrix_senso,short int *matrix_antisenso){
  // As matrizes já devem vir alocadas
  
  convert_target_to_graph(senso,strlen(senso),matrix_senso);
  convert_target_to_graph(antisenso,strlen(antisenso),matrix_antisenso);
  
  return;
}

char* get_antisenso(char *s){
	int i;
	char *antisenso;
	int a_size;
	
	a_size = strlen(s);
	antisenso = (char*)malloc((a_size+1)*sizeof(char));
	strcpy(antisenso,"");
	
	for(i=0;i<a_size;i++){
		switch(s[a_size - i-1]){
			case 'A':
				strcat(antisenso, "T");
			break;
			case 'C':
				strcat(antisenso, "G");
			break;
			case 'T':
				strcat(antisenso, "A");
			break;
			case 'G':
				strcat(antisenso, "C");
			break;
			default:
				strcat(antisenso,"N");
				break;
		}	
	}
	//strcat(antisenso,'\0');
	
	return antisenso;
}

extern "C" void setup_for_cuda(char *seq){
	// Recebe um vetor de caracteres com o padrão a ser procurado
	short int *h_matrix_senso;
	short int *h_matrix_antisenso;
	int size = strlen(seq);
	char *d_senso;
	char *d_antisenso;
	
	h_matrix_senso = (short int*)malloc(size*sizeof(short int));
	h_matrix_antisenso = (short int*)malloc(size*sizeof(short int));
	    
    //Configura grafos direto na memória da GPU
	set_grafo(seq,get_antisenso(seq),h_matrix_senso,h_matrix_antisenso);
	
	// Copia dados
	cudaMemcpyToSymbol(d_matrix_senso,h_matrix_senso,size*sizeof(short int),0,cudaMemcpyHostToDevice);
	cudaMemcpyToSymbol(d_matrix_antisenso,h_matrix_antisenso,size*sizeof(short int),0,cudaMemcpyHostToDevice);
	/*cudaMalloc((void**)&d_senso,(size+1)*sizeof(char));
	cudaMalloc((void**)&d_antisenso,(size+1)*sizeof(char));
	
	cudaMemcpy(d_senso,seq,(size+1)*sizeof(char),cudaMemcpyHostToDevice);
	cudaMemcpy(d_antisenso,get_antisenso(seq),(size+1)*sizeof(char),cudaMemcpyHostToDevice);
	*/
	//printf("Verificando matrizes:...\n");
	//check_matrix<<<1,size,0>>>(d_senso,d_antisenso);
	
	//printString("Grafo de busca configurado.",NULL);
	free(h_matrix_senso);
	free(h_matrix_antisenso);
	
	return;
}
