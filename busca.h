/* This file was automatically generated.  Do not edit! */
__host__ __device__ vgrafo *busca_vertice(char,vgrafo *,vgrafo *,vgrafo *,vgrafo *);
__global__ void k_busca(int *matchs,char **data,int,vgrafo *a,vgrafo *c,vgrafo *g,vgrafo *t);
__host__ __device__ void caminhar(vgrafo *,vgrafo *,int *,int *);
__device__ void build_grafo(vgrafo *,vgrafo *,vgrafo *,vgrafo *);
__global__ void set_grafo(char *,char *,vgrafo *,vgrafo *,vgrafo *,vgrafo *);
