//      aux.cu
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Arquivo com funções auxiliares para a execução da busca e análise da biblioteca
//
//		27/03/2012

#include "estruturas.h"
#include "load_data.h"
#include "operacoes.h"
void aux(int CUDA){
	
	int m;//Quantidade sequências
	int n;//Elementos por sequência
	int buffer_size = 2;
	Buffer b;
	char *tmp;
	
	//Inicializa
	get_setup(&m,&n);
	prepare_buffer(&b,buffer_size);
	
	while( check_file_end()== 0){
		//Realiza loop enquanto existirem sequências para encher o buffer
		fill_buffer(&b,n);
		 tmp = get_antisenso(b.seq[0]);
	}
	
}
