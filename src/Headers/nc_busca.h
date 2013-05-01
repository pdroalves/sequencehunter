#ifndef NC_BUSCA_H
#define NC_BUSCA_H
void convert_to_graph(char *s,int s_len,int *vector);
void busca(const int bloco1,const int bloco2,const int blocos,struct buffer *buffer,int*,int*,int *resultados,int *search_gaps);
void setup_without_cuda(char *seq);
#endif
