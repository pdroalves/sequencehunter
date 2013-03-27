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


extern "C" void checkCudaError();

////////////////////////////////////////////////////////////////////////////////////////
//////////////////					Buscador		 				////////////////////
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
///////////////				Metodo de busca com CUDA				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

__global__ void k_buscador(int totalseqs,
										int seqSize_an,
										char **data,
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
  ////////		data: o endereco com todo o buffer carregado
  ////////
  ////////
  ////////
  
  unsigned int seqId;// id da sequencia analisada
  int baseId;// id da base analisada
  short int tipo;// Variavel temporaria para salvar o resultado de uma analise
  short int linha;// Cada thread cuida de uma linha
  short int lsenso;// Guarda o valor da matriz senso para ser comparado com a sequencia alvo
  short int lantisenso;// Guarda o valor da matriz antisenso para ser comparado com a sequencia alvo
  short int alarmS;// Caso a comparacao do valor da matriz senso falhe, essa variavel encerra o loop
  short int alarmAS;// Caso a comparacao do valor da matriz antisenso falhe, essa variavel encerra o loop
  short int fase;// Guarda a posicao analisada
  short int seqSize_bu;// Tamanho da sequencia alvo
  char *seq;// Sequencia sob analise
  seqId = threadIdx.x + blockIdx.x*blockDim.x;
  seqSize_bu = bloco1+bloco2+blocoV;

	if(seqId < totalseqs){
	  tipo = 0;
	  fase = 0;
	  while(fase + seqSize_bu <= seqSize_an && !tipo){
			   seq = data[seqId]+fase;	
			   alarmS = 0;
			   alarmAS = 0;
			   // Quando esse loop for encerrado eu jah saberei se a sequencia eh senso, antisenso ou nada
			   for(baseId=0; 
						(baseId < seqSize_bu) && (!alarmS || !alarmAS); 
										baseId++){	
					linha = 0;
					// Carrega a linha analisada	
					lsenso = d_matrix_senso[baseId];
					lantisenso = d_matrix_antisenso[baseId];	
										
					linha = seq[baseId];
					
					// Verifica se algum alarme deve ser ativado			
					alarmS += (linha-lsenso)*(lsenso-'N');		
					alarmAS += (linha-lantisenso)*(lantisenso-'N');
				}
				
			// Guarda resultados
			if(!alarmS)
				tipo = SENSO;
			else 
				if(!alarmAS) 
					tipo = ANTISENSO;
			
			// Caso nao tenha encontrado nada, tenta pular para a base seguinte
			fase++;   
		}			
		
	   
		if(tipo == SENSO){
				search_gaps[seqId] = fase + bloco1 -1;
		}else  
			if(tipo == ANTISENSO){
				search_gaps[seqId] = fase + bloco2 -1;
			}
								 
		resultados[seqId] = tipo;	 
	}
	return;
}

extern "C" void k_busca(const int loaded,const int seqSize_an,const int seqSize_bu,int bloco1,int bloco2,int blocoV,char **data,short int *resultados,short int *search_gaps,char **founded,cudaStream_t stream){
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
	
	k_buscador<<<dimGrid,dimBlock,0>>>(loaded,seqSize_an,data,resultados,search_gaps,bloco1,bloco2,blocoV);
	
	checkCudaError();
	return;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
///////////////				Metodo de busca sem CUDA				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/*void failure_function(char *s,short int *table){
	short int pos = 2;
	short int cnd = 0;
	short int seq_size = strlen(s);
	
	table[0] = -1;
	table[1] = 0;
	
	while(pos < seq_size){
		if(s[pos-1] == s[cnd]){
			cnd++;
			table[pos] = cnd;
			pos++;
		}else if(cnd > 0){
			cnd = table[cnd];
		}else{
			table[pos] = 0;
			pos++;
		}
	} 
	
}*/


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
  
  getMatrix(matrix_senso,senso);
  getMatrix(matrix_antisenso,antisenso);
  
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
	cudaMalloc((void**)&d_senso,(size+1)*sizeof(char));
	cudaMalloc((void**)&d_antisenso,(size+1)*sizeof(char));
	
	cudaMemcpy(d_senso,seq,(size+1)*sizeof(char),cudaMemcpyHostToDevice);
	cudaMemcpy(d_antisenso,get_antisenso(seq),(size+1)*sizeof(char),cudaMemcpyHostToDevice);
	
	//printf("Verificando matrizes:...\n");
	//check_matrix<<<1,size,0>>>(d_senso,d_antisenso);
	
	//printString("Grafo de busca configurado.",NULL);
	free(h_matrix_senso);
	free(h_matrix_antisenso);
	
	return;
}
