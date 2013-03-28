#include <stdlib.h>
#include <string.h>
#include "../Headers/estruturas.h"
#include "../Headers/operacoes.h"
short int matrix_senso[MAX_SEQ_SIZE];
short int matrix_antisenso[MAX_SEQ_SIZE];

unsigned long get_hash(char* original,int start_offset,int end_offset){
	char *c;
	unsigned long hash = 5381;
	int i;

	for(i=start_offset;i < end_offset;i++)
		hash = ((hash << 5)+hash)+original[i];
	return hash;
}



void buscador(const int bloco1,const int bloco2,const int seqSize_bu,Buffer *buf,int *resultados,int *search_gaps,const int seqId){
  int baseId;// id da base analisada
  int tipo;
  int linha;// Cada thread cuida de uma linha
  int lsenso;
  int lantisenso;
  int alarmS;
  int alarmAS;
  int fase;
  const int seqSize_an = strlen(buf->seq[seqId]);
  //int tabela[MAX_SEQ_SIZE];
  char *seq;
  unsigned long seq_original_hash = get_hash(buf->seq[seqId]);
  
	  tipo = 0;
	  fase = 0;
	  while(fase + seqSize_bu <= seqSize_an && !tipo){
			   seq = buf->seq[seqId]+fase;	
			   //failure_function(seq,tabela);
			   alarmS = 0;
			   alarmAS = 0;
			   // Quando esse loop for encerrado eu jah saberei se a sequencia eh senso, antisenso ou nada
			   for(baseId=0; 
						(baseId < seqSize_bu) && (!alarmS || !alarmAS); 
										baseId++){
					// Carrega a linha relativa a base analisada	
					lsenso = matrix_senso[baseId];
					lantisenso = matrix_antisenso[baseId];
											
					linha = seq[baseId];	
					alarmS += (linha-lsenso)*(lsenso-'N');		
					alarmAS += (linha-lantisenso)*(lantisenso-'N');						
				}
			if(!alarmS)
				tipo = SENSO;
			else 
				if(!alarmAS) 
					tipo = ANTISENSO;
			
			fase++;  									
		}

		resultados[seqId] = tipo;	
		if(tipo == SENSO){
			//printf("%s -> s_match= %d e as_match=%d\n",seq,s_match,as_match);
			search_gaps[seqId] = fase + bloco1 -1;
		}else if(tipo == ANTISENSO){
			//printf("%s -> s_match= %d e as_match=%d\n",seq,s_match,as_match);
			search_gaps[seqId] = fase + bloco2 -1;
		}
	
	
	return;
}

void busca(const int bloco1,const int bloco2,const int blocos,Buffer *buffer,int *resultados,int *search_gaps){
	int i;
	int size;
	
	size = buffer->load;
	
	for(i=0; i < size; i++)
		buscador(bloco1,bloco2,blocos,buffer,resultados,search_gaps,i);//Metodo de busca
		
	return;
}

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

void setup_without_cuda(char *seq){
// Recebe um vetor de caracteres com o padrão a ser procurado
	int size = strlen(seq);
	
    //Configura grafos direto na memória da GPU
	set_grafo(seq,get_antisenso(seq),matrix_senso,matrix_antisenso);
	

	return;
}
