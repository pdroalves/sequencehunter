//      operacoes.c
//      
//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
//      
//		Contem funções e métodos comuns ao projeto Sequence Hunter
//
//		27/03/2012

#include<stdio.h>

int conta_posicoes(char *seq){
	//Retorna o tamanho a string seq
 int i=0;
 while(seq[i] != \0){
		i++;
   }
   return i+1;
}
