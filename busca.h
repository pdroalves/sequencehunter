/* This file was automatically generated.  Do not edit! */

#ifndef BUSCA_H
#define BUSCA_H
void busca(const int bloco1,const int bloco2,const int blocos,struct buffer *buffer,int *resultados,int *search_gaps);
void k_busca(const int loaded,const int seqSize_an,const int seqSize_bu,int bloco1,int bloco2,int blocoV,char **data,short int *resultados,short int *search_gaps,char **founded,cudaStream_t stream);
void checkCudaError();
void setup_for_cuda(char *seq);
void setup_without_cuda(char *seq);
#endif
