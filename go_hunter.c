//      aux.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include "cuda.h"
#include "cuda_runtime_api.h"
#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
#include "busca.h"
#include "log.h"
#include "fila.h"
#include "go_hunter_cuda.h"
#include "go_hunter_noncuda.h"
	
GHashTable* aux(int CUDA,char *c,const int bloco1,const int bloco2,const int blocos,gboolean disable_cuda,gboolean sil,gboolean verb){
	if(CUDA)
		return auxCUDA(c,bloco1,bloco2,blocos,verb,sil);
	else
		return auxNONcuda(c,bloco1,bloco2,blocos,verb,sil);
}

