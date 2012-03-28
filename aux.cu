#include "estruturas.h"
#include "load_data.h"

void aux(int CUDA){
	
	int m;//Quantidade sequências
	int n;//Elementos por sequência
	int buffer_size = 2;
	Buffer b;
	
	//Inicializa
	get_setup(&m,&n);
	prepare_buffer(&b,buffer_size);
	
	while( check_file_end()== 0){
		//Realiza loop enquanto existirem sequências para encher o buffer
		fill_buffer(&b,n);
	}
	
}
