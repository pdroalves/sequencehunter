#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "estruturas.h"
#include "gthread_funcs.h"
#include "pilha.h"

// Buffer_flag:
//		0: processamento em andamento
//		1: enchendo buffer
//		-1: processo terminado

IteratorPack* get_pack(Buffer *buffer,int *buffer_flag,pilha *p_sensos,pilha *p_antisensos,int bloco1,int bloco2,int blocos,int n,vgrafo *d_a,vgrafo *d_c,vgrafo *d_g,vgrafo *d_t,gboolean verbose,gboolean silent){
	
	IteratorPack *ip;
	
	ip = (IteratorPack*)malloc(sizeof(IteratorPack));
	
	ip->buffer = buffer;
	ip->buffer_flag = *buffer_flag;
	ip->sensos = p_sensos;
	ip->antisensos = p_antisensos;
	ip->bloco1 = bloco1;
	ip->bloco2 = bloco2;
	ip->blocos = blocos;
	ip->n = n;
	ip->a = d_a;
	ip->c = d_c;
	ip->g = d_g;
	ip->t = d_t;
	ip->verbose = verbose;
	ip->silent = silent;
	return ip;
}

void* iterator(void *args){
	IteratorPack *ip;
	int i;
	int p=0;
	int bloco1;
	int bloco2;
	int blocos;
	int n;
	int load;
	vgrafo *a;
	vgrafo *c;
	vgrafo *g;
	vgrafo *t;
	char *tmp;
	pilha *p_sensos;
	pilha *p_antisensos;
	pilha *novo;
	Buffer *buffer;
	gboolean verbose;
	gboolean silent;

	//Carrega pilhas
	p_sensos = criar_pilha();
	p_antisensos = criar_pilha();
	
	//Abre pacote
	ip = (IteratorPack*) args;
	buffer = ip->buffer;
	bloco1 = ip->bloco1;
	bloco2 = ip->bloco2;
	blocos = ip->blocos;
	n = ip->n;
	a = ip->a;
	c = ip->c;
	g = ip->g;
	t = ip->t;
	verbose = ip->verbose;
	silent = ip->silent;
	
	while( buffer->load == 0){
		//g_cond_wait(buffer_filled_cond,flag_mutex);
	}//Aguarda para que o buffer seja enchido pela primeira vez
  
	while(buffer->load != -1){
				
		//Realiza loop enquanto existirem sequências para encher o buffer	
		busca(bloco1,bloco2,blocos,buffer,1,1,a,c,g,t);//Kernel de busca
		
		p += buffer->load;
		load = buffer->load;
		//printf("%d\n",p);
		for(i = 0; i < load;i++){//Copia sequências senso e antisenso encontradas
			switch(buffer->resultado[i]){
				case 1:
				tmp = buffer->seq[i];
				if(verbose == TRUE && silent != TRUE)	
				  printf("S: %s - %d\n",tmp,p);
				novo = criar_elemento_pilha(tmp);
				empilha(&p_sensos,novo);
				//printString("Senso:",tmp);
				buffer->load--;
				break;
				case 2:
				tmp = buffer->seq[i];
				if(verbose == TRUE && silent != TRUE)
				  printf("N: %s - %d\n",tmp,p);
				novo = criar_elemento_pilha(tmp);
				empilha(&p_antisensos,novo);
				//printString("Antisenso:",tmp);
				buffer->load--;
				break;
				default:
					buffer->load--;
				break;
			}
		}
											
		if(buffer->load != 0){
			printf("Erro! Buffer não foi totalmente esvaziado.\n");
			buffer->load = 0;
		}
		
		//g_mutex_lock(flag_mutex);
		//g_cond_signal(buffer_vazio_cond);				
		//g_mutex_unlock(flag_mutex);
	  	
	  	//g_mutex_lock(flag_mutex);
		while( buffer->load == 0){
		//	g_cond_wait(buffer_filled_cond,flag_mutex);
		}//Aguarda para que o buffer seja enchido
		//g_mutex_unlock(flag_mutex);
	  		
	}
	
	g_mutex_lock(senso_mutex);
	empilhar_pilha(ip->sensos,&p_sensos);
	g_mutex_unlock(senso_mutex);		
	
	g_mutex_lock(antisenso_mutex);
	empilhar_pilha(ip->antisensos,&p_antisensos);
	g_mutex_unlock(antisenso_mutex);
	//g_thread_exit(NULL);
	return;
}
