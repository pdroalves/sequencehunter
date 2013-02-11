//      busca.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Implementação do algoritmo de busca por grafos.
//
//		27/03/2012

#include <stdio.h>
#include <cuda.h>
#include "../Headers/estruturas.h"
#include "../Headers/cuda_functions.h"
#include "../Headers/log.h"

#define ABSOLUTO(a) a>=0?a:-a
short int matrix_senso[MAX_SEQ_SIZE];
short int matrix_antisenso[MAX_SEQ_SIZE];

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
										char **founded,
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
  short int i;
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
											
					// Conversao de char para inteiro
					switch(seq[baseId]){
						case 'A':
							linha = A;	
						break;
						case 'C':
							linha = C;		
						break;
						case 'G':
							linha = G;	
						break;
						case 'T':
							linha = T;
						break;
						default:
							linha = N;
						break;
					}
					
					// Verifica se algum alarme deve ser ativado			
					alarmS += (linha-lsenso)*(lsenso-N);		
					alarmAS += (linha-lantisenso)*(lantisenso-N);
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
		
	   
		if(tipo == SENSO)
			 for(i=0;i<seqSize_an;i++)
					founded[seqId][i] = data[seqId][i];
		else  
			if(tipo == ANTISENSO)
				for(i=0;i<seqSize_an;i++)
					founded[seqId][i] = data[seqId][i];
			
								 
		resultados[seqId] = tipo;	 
		search_gaps[seqId] = fase-1;
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
	
	k_buscador<<<dimGrid,dimBlock,0,stream>>>(loaded,seqSize_an,data,resultados,search_gaps,founded,bloco1,bloco2,blocoV);
	
	checkCudaError();
	return;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
///////////////				Metodo de busca sem CUDA				////////////////////
////////////////////////////////////////////////////////////////////////////////////////

extern "C" __host__ void buscador(const int bloco1,const int bloco2,const int seqSize_bu,Buffer *buffer,int *resultados,int *search_gaps,const int seqId){
  int baseId;// id da base analisada
  short int tipo;
  short int linha;// Cada thread cuida de uma linha
  short int lsenso;
  short int lantisenso;
  short int alarmS;
  short int alarmAS;
  short int fase;
  const short int seqSize_an = strlen(buffer->seq[seqId]);
  const short int blocoZ = seqSize_bu - bloco1 - bloco2 + 1;
  char *seq;  
  int i;
  
	  tipo = 0;
	  fase = 0;
	  while(fase + seqSize_bu <= seqSize_an && !tipo){
			   seq = buffer->seq[seqId]+fase;	
			   alarmS = 0;
			   alarmAS = 0;
			   // Quando esse loop for encerrado eu jah saberei se a sequencia eh senso, antisenso ou nada
			   for(baseId=0; 
						(baseId < seqSize_bu) && (!alarmS || !alarmAS); 
										baseId++){
					// Carrega a linha relativa a base analisada		
					linha = 0;
					lsenso = matrix_senso[baseId];
					lantisenso = matrix_antisenso[baseId];
											
					switch(seq[baseId]){
						case 'A':
							linha = A;	
						break;
						case 'C':
							linha = C;		
						break;
						case 'G':
							linha = G;	
						break;
						case 'T':
							linha = T;
						break;
						default:
							linha = N;
						break;
					}
								
					alarmS += (linha-lsenso)*(lsenso-N);		
					alarmAS += (linha-lantisenso)*(lantisenso-N);	
					
				}
			if(!alarmS)
				tipo = SENSO;
			else 
				if(!alarmAS) 
					tipo = ANTISENSO;
			
			fase++;   
									
		
		resultados[seqId] = tipo;	
		if(tipo == SENSO){
			//printf("%s -> s_match= %d e as_match=%d\n",seq,s_match,as_match);
			for(i=0;i<blocoZ;i++){
				  seq[i] = seq[i];
			}
			search_gaps[seqId] = fase + bloco1 -1;
		}else if(tipo == ANTISENSO){
			//printf("%s -> s_match= %d e as_match=%d\n",seq,s_match,as_match);
			for(i=0;i<blocoZ;i++){
				  seq[i] = seq[i];
			}			 
			search_gaps[seqId] = fase + bloco2 -1;
		}
	
	}
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

extern "C" void busca(const int bloco1,const int bloco2,const int blocos,Buffer *buffer,int *resultados,int *search_gaps){
	int i;
	int size;
	
	size = buffer->load;
	
	for(i=0; i < size; i++)
		buscador(bloco1,bloco2,blocos,buffer,resultados,search_gaps,i);//Metodo de busca
		
	return;
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
int getLine(char c){
	// Recebe uma base e retorna uma linha de binarios
	
	switch(c){
		case 'A':
			return A;
		case 'C':
			return C;
		case 'G':
			return G;
		case 'T':
			return T;
		default:
			return N;
	}
}

void getMatrix(short int *matrix,char *str){
	// Matrix já deve vir alocada
	int size_y;
	int i;

	size_y = strlen(str);

	// Preenche matriz
	for(i = 0; i < size_y;i++){
		matrix[i] = getLine(str[i]);
	}	

	return;
}

 void set_grafo_CUDA(char *senso,char *antisenso,short int *matrix_senso,short int *matrix_antisenso){
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

__global__ void check_matrix(char *senso,char *antisenso){
	// Verifica se a matriz montada corresponde a sequencia desejada
	// Devem haver N threads e 1 bloco para uma sequencia de tamanho N
	
	__shared__ int alarmS;
	__shared__ int alarmAS;
	char cS;
	char cAS;
	int id = threadIdx.x;
	int e;
	
	cS = senso[threadIdx.x];
	cAS = antisenso[threadIdx.x];
	alarmS = 0;
	alarmAS = 0;
	
    switch(cS){
		case 'A':
			e = A;	
		break;
		case 'C':
			e = C;		
		break;
		case 'G':
			e = G;	
		break;
		case 'T':
			e = T;
		break;
		default:
			e = N;
		break;
	}	
	
	// Confere Senso
	if(d_matrix_senso[id] != e) alarmS = 1;
	
	switch(cAS){
		case 'A':
			e = A;	
		break;
		case 'C':
			e = C;		
		break;
		case 'G':
			e = G;	
		break;
		case 'T':
			e = T;
		break;
		default:
			e = N;
		break;
	}	
	
	// Confere Antisenso	
	if(d_matrix_antisenso[id] != e) alarmAS = 1;	
			
	__syncthreads();
	
	if(threadIdx.x == 0){
		if(alarmS)
			printf("Erro! Matriz senso montada incorretamente.\n");					
		else
			printf("Matriz senso montada corretamente.\n");
			
		if(alarmAS)
			printf("Erro! Matriz antisenso montada incorretamente.\n");
		else
			printf("Matriz antisenso montada corretamente.\n");
	}
		
	return;
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
	set_grafo_CUDA(seq,get_antisenso(seq),h_matrix_senso,h_matrix_antisenso);
	
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


extern "C" void set_grafo_NONCuda(char *senso,char *antisenso,short int *matrix_senso,short int *matrix_antisenso){									
  
  getMatrix(matrix_senso,senso);
  getMatrix(matrix_antisenso,antisenso);
  return;
}

extern "C"  void setup_without_cuda(char *seq){
// Recebe um vetor de caracteres com o padrão a ser procurado
	int size = strlen(seq);
	
    //Configura grafos direto na memória da GPU
	set_grafo_NONCuda(seq,get_antisenso(seq),matrix_senso,matrix_antisenso);
	

	return;
}
