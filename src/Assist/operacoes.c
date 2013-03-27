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
#include <omp.h>
#include <sys/sysinfo.h>
#include "../Headers/processing_data.h"
#define TAM_MAX 10000


#ifdef _WIN32
#include <windows.h>
size_t getTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#else
#include <unistd.h>
size_t getTotalSystemMemory()
{
	FILE *meminfo = fopen("/proc/meminfo", "r");

    char line[256];
    while(fgets(line, sizeof(line), meminfo))
    {
        int ram;
        if(sscanf(line, "MemTotal: %d kB", &ram) == 1)
        {
            fclose(meminfo);
            return ram;
        }
    }

    // If we got here, then we couldn't find the proper line in the meminfo file:
    // do something appropriate like return an error code, throw an exception, etc.
    fclose(meminfo);
    return -1;
}
#endif

char* get_antisenso(char *s){
	int i;
	char *antisenso;
	int a_size;
	if(s != NULL){
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
	}else{
		return NULL;
	}
}

int check_base_valida(char c){
	if(c == 'A' || c == 'C' || c == 'T' || c == 'G') return 1;
	else return 0;
}

int get_sequencias_validas(FILE **f,int files){
	int j = 0;
	int notify_tamanho_variavel = 0;
	int seqs_validas = 0;
	int m = 0;
	int n = 0;
	char *tmp;
	
	#pragma omp parallel shared(files) shared(seqs_validas)
	{
		tmp = (char*)malloc(TAM_MAX*sizeof(char));
		for(j=0;j < files;j++){		
			while(feof(f[j]) == 0){
					fscanf(f[j],"%s",tmp);	
					n = strlen(tmp);
					if(check_seq_valida(tmp)){	
						seqs_validas++;
						//printf("%d\n",seqs_validas);
						if(m == 0) m = n;
						else if(n!=m && notify_tamanho_variavel == 0) notify_tamanho_variavel = 1;
					}
					tmp[0] = '\0';
			}
		}
	}
	for(j=0;j<files;j++) rewind(f[j]);
	return seqs_validas;
}

	
