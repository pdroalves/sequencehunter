/* This file was automatically generated.  Do not edit! */

#ifndef BUSCA_H
#define BUSCA_H
vgrafo *busca_vertice(char,vgrafo *,vgrafo *,vgrafo *,vgrafo *);
void checkCudaError();
void set_grafo_helper(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
void k_busca_helper(int num_blocks,int num_threads,const int bloco1,const int bloco2,const int blocos,char **s,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t);
void busca(const int bloco1,const int bloco2,const int blocos,Buffer *buffer,int *resultados,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
void k_busca(int num_blocks,int num_threads,const int bloco1,const int bloco2,const int blocos,char **data,int *resultados,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t);
void caminhar(vgrafo*,vgrafo *,vgrafo *,int *,int *);
void build_grafo(vgrafo *,vgrafo *,vgrafo *,vgrafo *);
void set_grafo(char *,char *,vgrafo *,vgrafo *,vgrafo *,vgrafo *);
void destroy_grafo(vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
void set_grafo_NONCuda(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
#endif
