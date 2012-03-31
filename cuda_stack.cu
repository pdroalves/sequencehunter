//      cuda_stack.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Recupera as sequências encontradas e salvas na memória da GPU
//
//		31/03/2012

#include <stdio.h>
#include <cuda.h>
#include "estruturas.h"

__global__ void k_recupera(char **data,char **senso,char **antisensos){
	
  ////////
  ////////	
  ////////		Esse kernel deve deve ser executado por dois threads, um para o senso e o outro para o antisenso
  ////////		Recebe endereços na memória global para que as sequências senso e antisenso sejam salvas
  ////////		Recebe os endereços originais onde todas as sequências foram salvas
  ////////
  ////////
  ////////
  ////////
  
  const int posicao = blockIdx.x*blockDim.x + threadIdx.x;
  
  switch(data[posicao][0]){
	  case 'S':
		//Senso
		senso[posicao] = data[posicao][1];
		break;
	  case 'N':
		//Antisenso
		antisenso[posicao] = data[posicao][1];
		break;
	return;
}
