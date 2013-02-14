//      aux.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include <glib.h>
#include "../Headers/estruturas.h"
#include "../Headers/go_hunter_cuda.h"
#include "../Headers/go_hunter_noncuda.h"
void aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos,Params set){
	if(CUDA)
		auxCUDA(c,bloco1,bloco2,blocos,set);
	else
		auxNONcuda(c,bloco1,bloco2,blocos,set);
    return; 
}
