/* This file was automatically generated.  Do not edit! */

#ifndef BUSCA_H
#define BUSCA_H
void k_busca(const int loaded,const int seqSize_an,const int seqSize_bu,int bloco1,int bloco2,int blocoV,char **data,short int *resultados,short int *search_gaps,char **founded,cudaStream_t stream);
void checkCudaError();
void setup_for_cuda(char *seq);
#endif
