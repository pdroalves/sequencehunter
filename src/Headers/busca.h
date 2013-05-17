/* This file was automatically generated.  Do not edit! */

#ifndef BUSCA_H
#define BUSCA_H
void cuda_convert_to_graph(char *s,int s_len,int *vector);
void k_busca(const int loaded,const int seqSize_an,const int seqSize_bu,int bloco1,int bloco2,int blocoV,short int*,short int*,short int *resultados,short int *search_gaps,cudaStream_t stream);
void checkCudaError();
void setup_for_cuda(char *seq);
#endif
