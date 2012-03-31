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

__global__ void k_recupera(char **data,char **senso,char **antisenso){
	
  ////////
  ////////	
  ////////		Esse kernel deve deve ser executado pela mesma quantidade de threads que processaram o kernel k_busca.
  ////////		Recebe endereços na memória global para que as sequências senso e antisenso sejam salvas.
  ////////		Recebe os endereços originais onde todas as sequências foram salvas.
  ////////
  ////////
  ////////
  ////////
  
  const int posicao = blockIdx.x*blockDim.x + threadIdx.x;
  
  //printf("%s\n",data[posicao]);
  switch(data[posicao][0]){
	  case 'S':
		//Senso
		senso[posicao] = data[posicao];
		break;
	  case 'N':
		//Antisenso
		antisenso[posicao] = data[posicao];
		break;
		default:
		senso[posicao][0] = '\0';
		antisenso[posicao][0] = '\0';
		break;
	}
	
	return;
}
