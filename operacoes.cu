	//      operacoes.c
	//      
	//      Copyright 2012 Pedro Alves <pdroalves@gmail.com>
	//      
	//		Contem funções e métodos comuns ao projeto Sequence Hunter
	//
	//		27/03/2012

	#include <stdio.h>
	#include <glib.h>

	char* get_antisenso(char *s){
		int i;
		char *antisenso;
		int a_size;
		
		a_size = strlen(s);
		antisenso = (char*)malloc(a_size*sizeof(char));
		
		for(i=0;i<a_size;i++){
			switch(s[a_size - i -1]){
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
			}	
		}
		return antisenso;
	}

	int escolhe_GPU(){
	  int num_devices, device,max_multiprocessors,max_device;
	  cudaDeviceProp properties;	

	  cudaGetDeviceCount(&num_devices);
	  max_device = 0;
		
	  if (num_devices > 1) {
			max_multiprocessors = 0;
			  
			for (device = 0; device < num_devices; device++) {//Busca a melhor GPU comparando a quantidade de multi processadores           
			  cudaGetDeviceProperties(&properties, device);
			  if (max_multiprocessors < properties.multiProcessorCount) {
				max_multiprocessors = properties.multiProcessorCount;
				max_device = device;
			  }
			}
			cudaSetDevice(max_device);
		  }
			
		  return max_device;
		}

	void getDevice(int deviceCount,cudaDeviceProp deviceProp,gboolean verbose){

	  int i;
	  int device;
	  cudaError_t erro;

		 printf("Dispositivos encontrados:\n");
		  for(i=0;i<deviceCount;i++)
			{
			  erro = cudaGetDeviceProperties(&deviceProp,i);
			  g_assert(erro == cudaSuccess);
			  printf("%d) %s\n",i,deviceProp.name);
			}

		  printf("\n Usar dispositivo:");
		  scanf("%d",&device);
			
		  while(device > deviceCount)
			{
			  printf("Dispositivo invalido\n");
			  scanf("%d",&device);
			}
		
		//Libera o dispositivo em uso
		erro = cudaThreadExit();
		g_assert(erro == cudaSuccess);
		
		//Configura qual dispositivo deve ser usado
		erro = cudaSetDevice(device);
		g_assert(erro == cudaSuccess);
			
		//O dispositivo foi escolhido
		erro = cudaGetDeviceProperties(&deviceProp,device);
		g_assert(erro == cudaSuccess);
		
		if(verbose == TRUE) printf("Dispositivo configurado para uso: %s\n\n",deviceProp.name); 
	 
	}
