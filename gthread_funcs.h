#ifndef GTHREAD_FUNCS_H
#define GTHREAD_FUNCS_h

GMutex *flag_mutex;//Controla buffer_flag
GMutex *senso_mutex;//Controla a pilha global de sensos
GMutex *antisenso_mutex;//Controla a pilha global de antisensos

GCond *buffer_filled_cond;
GCond *buffer_vazio_cond;

struct iterator_pack{
	Buffer *buffer;
	int buffer_flag;
	pilha *sensos;
	pilha *antisensos;
	int bloco1;
	int bloco2;
	int blocos;
	int n;
	gboolean verbose;
	gboolean silent;
	vgrafo *a;
	vgrafo *c;
	vgrafo *g;
	vgrafo *t;
};
typedef struct iterator_pack IteratorPack;

IteratorPack* get_pack(Buffer *buffer,int *buffer_flag,pilha *p_sensos,pilha *p_antisensos,int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t,gboolean verbose,gboolean silent);
void* iterator(void *args);
#endif
