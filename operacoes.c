	//      operacoes.c
	//      
	//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
	//      
	//		Contem funções e métodos comuns ao projeto Sequence Hunter
	//
	//		27/03/2012
	#include <stdlib.h>
	#include <string.h>
	#include <stdio.h>

	char* get_antisenso(char *s){
		int i;
		char *antisenso;
		int a_size;
		
		a_size = strlen(s);
		antisenso = (char*)malloc((a_size+1)*sizeof(char));
		strcpy(antisenso,"");
		
		for(i=0;i<a_size;i++){
			switch(s[a_size - i-1]){
				case 'A':
					strcat(antisenso, "T");
				break;
				case 'C':
					strcat(antisenso, "G");
				break;
				case 'T':
					strcat(antisenso, "A");
				break;
				case 'G':
					strcat(antisenso, "C");
				break;
				default:
					strcat(antisenso,"N");
				break;
			}	
		}
		//strcat(antisenso,'\0');
		
		return antisenso;
	}
	
	int check_base_valida(char c){
		if(c == 'A' || c == 'C' || c == 'T' || c == 'G') return 1;
		else return 0;
	}

	
