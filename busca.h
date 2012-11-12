/* This file was automatically generated.  Do not edit! */

#ifndef BUSCA_H
#define BUSCA_H
struct vertice_grafo *busca_vertice(char,struct vertice_grafo *,struct vertice_grafo *,struct vertice_grafo *,struct vertice_grafo *);
void set_grafo_helper(char *senso,char *antisenso,int **d_matrix_senso,int **d_matrix_antisenso);
void k_busca_helper(int num_blocks,int num_threads,const int bloco1,const int bloco2,const int blocos,char **s,struct vertice_grafo *d_a,struct vertice_grafo *d_c,struct vertice_grafo *d_g,struct vertice_grafo *d_t);
void busca(const int bloco1,const int bloco2,const int blocos,struct buffer *buffer,int *resultados,struct vertice_grafo *a,struct vertice_grafo *c,struct vertice_grafo *g, struct vertice_grafo *t);
void k_busca(int loaded,int num_blocks,int num_threads,const int bloco1,const int bloco2,const int blocos,char **data,int *resultados,char **founded,struct vertice_grafo *d_a,struct vertice_grafo *d_c,struct vertice_grafo *d_g,struct vertice_grafo *d_t,cudaStream_t stream);
void caminhar(struct vertice_grafo*,struct vertice_grafo *,struct vertice_grafo *,int *,int *);
void build_grafo(struct vertice_grafo *,struct vertice_grafo *,struct vertice_grafo *,struct vertice_grafo *);
void set_grafo(char *,char *,struct vertice_grafo *,struct vertice_grafo *,struct vertice_grafo *,struct vertice_grafo *);
void destroy_grafo(struct vertice_grafo *a,struct vertice_grafo *c,struct vertice_grafo *g, struct vertice_grafo *t);
void checkCudaError();
void set_grafo_NONCuda(char *senso,char *antisenso,struct vertice_grafo *a,struct vertice_grafo *c,struct vertice_grafo *g, struct vertice_grafo *t);
char** cudaGetArrayOfArraysChar(int narrays,int arrays_size);
void cudaCopyCharArrays(char **src,char **dst,int n);
#endif
