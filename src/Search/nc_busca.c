#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Headers/estruturas.h"
#include "../Headers/operacoes.h"

int matrix_senso[MAX_SEQ_SIZE];
int matrix_antisenso[MAX_SEQ_SIZE];

void convert_to_graph(char *s,int s_len,int *vector){
  int i;
  for(i=0;i<s_len-1;i++)
    vector[i] = s[i]*(2+s[i+1]);
  return;
}

void convert_target_to_graph(char *s,int s_len,int *vector){
  int i;
  for(i=0;i<s_len-1;i++)
    if(s[i] == 'N' || s[i+1] == 'N')
      vector[i] = -1*s[i]*(2+s[i+1]);
    else
      vector[i] = s[i]*(2+s[i+1]);
  return;
}

int get_candidate_table(int start_vertex,int *vertexes,int v_size,int *table){
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

int match_check(int *target,int target_size,int *analyse){
  int i;
  for(i=0;i<target_size;i++){
    if(target[i] >= 0)
      if(target[i] != analyse[i])
        return 0;
  }
  return 1;
}

void buscador(  const int bloco1,
                const int bloco2,
                const int seqSize_bu,
                const int seqSize_an,
                const Buffer *buf,
                const int *vertexes,
                int *candidates,
                int *resultados,
                int *search_gaps,
                const int seqId){  
  
  const char *seq = buf->seq[seqId];
  int *this_candidates = &candidates[seqId*seqSize_an];
  int *this_vertexes = &vertexes[seqId*seqSize_an];
  int num_sensos_candidates;
  int num_antisensos_candidates;
  int i;
  int candidate_pos_sensos;
  int candidate_pos_antisensos;
  int tipo;
  int tmp;

  /*if( seq[0]*(2+seq[1]) != this_vertexes[0]){
    printf("Erro na %d - %d == %d em %d => %s\n",seqId,this_vertexes[0],vertexes[seqId*seqSize_an],seqId*seqSize_an,seq);
    exit(1);
  }
  convert_to_graph(seq,seqSize_an,this_vertexes);
*/
  num_sensos_candidates = get_candidate_table(matrix_senso[0],this_vertexes,seqSize_an-seqSize_bu+1,this_candidates);
  tipo = 0;
  for(i=0;i<num_sensos_candidates && !tipo;i++){
    candidate_pos_sensos = this_candidates[i];
    tmp = match_check(matrix_senso,seqSize_bu,&this_vertexes[candidate_pos_sensos]);
    if(tmp){
      search_gaps[seqId] = i + bloco1;
      tipo = SENSO;  
    }
  }
  if(!tipo){
    num_antisensos_candidates = get_candidate_table(matrix_antisenso[0],this_vertexes,seqSize_an-seqSize_bu+1,this_candidates);
    for(i=0;i<num_antisensos_candidates && !tipo;i++){
      candidate_pos_antisensos = this_candidates[i];
      if(match_check(matrix_antisenso,seqSize_bu,&this_vertexes[candidate_pos_antisensos])){
        search_gaps[seqId] = i + bloco2;
        tipo = ANTISENSO;  
        //printf("Encontrei um antisenso %s\n",seq);
      }
    }
  }
 /* if(strcmp("AGCTGAAAACT",seq) == 0){
      printf("Candidado falhou %d =>%s\n",seqId,seq);
    for(i=0;i<seqSize_an;i++){
      printf("%d ",this_vertexes[i]);
    }
    printf("\n");
    
  }*/
  resultados[seqId] = tipo;               
  return;
}

void busca( const int bloco1,
            const int bloco2,
            const int seqSize_bu,
            Buffer *buffer,
            int *vertexes,
            int *candidates,
            int *resultados,
            int *search_gaps){
	int i;
	int seqSize_an = strlen(buffer->seq[0]);

	for(i=0; i < buffer->load; i++)
		buscador( bloco1,
              bloco2,
              seqSize_bu,
              seqSize_an,
              buffer,
              vertexes,
              candidates,
              resultados,
              search_gaps,
              i);//Metodo de busca
	
	return;
}


void set_grafo(char *senso,char *antisenso,int *matrix_senso,int *matrix_antisenso){
  // As matrizes já devem vir alocadas
	
  convert_target_to_graph(senso,strlen(senso),matrix_senso);
  convert_target_to_graph(antisenso,strlen(antisenso),matrix_antisenso);
	
	return;
}

void setup_without_cuda(char *seq){
  // Recebe um vetor de caracteres com o padrão a ser procurado
	set_grafo(seq,get_antisenso(seq),matrix_senso,matrix_antisenso);	

	return;
}
