/* This file was automatically generated.  Do not edit! */

__host__ __device__ vgrafo *busca_vertice(char,vgrafo *,vgrafo *,vgrafo *,vgrafo *);
void set_grafo_helper(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
void k_busca_helper(int num_blocks,int num_threads,const int bloco1,const int bloco2,const int blocos,char **s,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t);
void busca(const int bloco1,const int bloco2,const int blocos,Buffer buffer,const int th_id,const int nthreads,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
__host__ __device__ void caminhar(vgrafo*,vgrafo *,vgrafo *,int *,int *);
__device__ void build_grafo(vgrafo *,vgrafo *,vgrafo *,vgrafo *);
__global__ void set_grafo(char *,char *,vgrafo *,vgrafo *,vgrafo *,vgrafo *);
void destroy_grafo(vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);
void set_grafo_NONCuda(char *senso,char *antisenso,vgrafo *a,vgrafo *c,vgrafo *g, vgrafo *t);

