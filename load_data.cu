#include <stdio.h>
#include <glib.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "estruturas.h"
#include "log.h"

#define TAM_MAX 10000

void get_setup(int*);	
void fill_buffer(struct buffer*,int);
int check_seq(char*,int*,int*,int*);
void prepare_buffer(struct buffer*,int);
void close_file();
int open_file(char **entrada,int);
int gpuDeviceInit(int devID);
inline int _ConvertSMVer2Cores(int major, int minor);
int gpuGetMaxGflopsDeviceId();
int findCudaDevice();
int check_gpu_mode();

FILE **f;
int files = 0;
/* converts integer into string */

int check_gpu_mode(){
	
	return gpuDeviceInit(findCudaDevice());
}


char* itoa(unsigned long num) {
        char* retstr = (char*)calloc(12, sizeof(char));
        if (sprintf(retstr, "%ld", num) > 0) {
                return retstr;
        } else {
                return NULL;
        }
}

int check_seq(char *seq,int *bloco1,int *bloco2,int *blocoV){
	int i;
	int tam;
	int tmp;
	int inv;
	
	*bloco1 = *bloco2 = *blocoV = -1;
	i = 0;
	tam = strlen(seq);
	inv = tmp = -1;
	
	while(i < tam && *bloco1 == -1){
		if(seq[i] == 'N')
			*bloco1 = i;
		i++;	
	}
	while(i < tam && tmp == -1){
		if(seq[i] != 'N')
			tmp = i;
		i++;	
	}
	*bloco2 = tam - tmp;
	*blocoV = tam - *bloco1 - *bloco2;
	while(i < tam && tmp != -1){
		if(seq[i] == 'N')
			inv = i;
		i++;
	}			
	
	if(*bloco1 == -1 || *bloco2 == -1 || inv != -1)
		return 0;
	return 1;
}

int open_file(char **entrada,int qnt){
	int checks[qnt];
	int i;
	int abertos = 0;
	int tmp = 0;
	f = (FILE**)malloc(qnt*sizeof(FILE*));
	while(files < qnt && abertos+1 < qnt){
		f[files] = fopen(entrada[abertos+1],"r+");
		checks[files] = f[files]!=NULL;
		if(checks[files] == 0){
			printf("Arquivo %s não pode ser aberto.\n",entrada[files+1]);
			abertos++;
		}else{
			printf("Arquivo %s aberto.\n",entrada[abertos+1]);
			print_open_file(entrada[abertos+1]);
			files++;
			abertos++;
		}
	}
	for(i=0;i<qnt;i++) tmp += checks[files];
	return tmp==qnt;
}

void close_file(){
	int i;
	for(i=0;i<files;i++)
		fclose(f[i]);
	return;
}

void get_setup(int *n){
	char *tmp;
	//Suponho que todas as sequências nas bibliotecas tem o mesmo tamanho
	tmp = (char*)malloc(TAM_MAX*sizeof(char));
	fscanf(f[0],"%s",tmp);
	rewind(f[0]);
	*n = (int)(strlen(tmp));
	free(tmp);
	return;
}

void prepare_buffer(Buffer *b,int c){
	b->capacidade = c;
	b->seq = (char**)malloc(c*sizeof(char*));
	b->load = 0;
	printString("Buffer configurado para: ",itoa(c));
	return;
}

void fill_buffer(Buffer *b,int n){
	int i = 0;
	int j = 0;
	for(j=0;j < files && i < b->capacidade;j++){		
		while(i < b->capacidade && feof(f[j]) == 0){
				b->seq[i] = (char*)malloc((n+1)*sizeof(char));
				fscanf(f[j],"%s",b->seq[i]);
				strcat(b->seq[i],"\0");
				i++;
		}
		b->load = i;	
		if(i < b->capacidade && i!=0){ 
			b->load--;
			i = b->load;
		}
		if(feof(f[files-1]) == 1 && b->load == 0) b->load = -1;//Não há mais arquivos
	}
	return;
}

	//#######################
	
	// General GPU Device CUDA Initialization
int gpuDeviceInit(int devID)
{
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0)
    {
        fprintf(stderr, "gpuDeviceInit() CUDA error: no devices supporting CUDA.\n");
        return 0;
    }

    if (devID < 0)
       devID = 0;
        

    cudaDeviceProp deviceProp;
   cudaGetDeviceProperties(&deviceProp, devID);

    if (deviceProp.major < 2)
    {
        fprintf(stderr, "gpuDeviceInit(): GPU device does not support CUDA. Revision < 2.0.\n");
        return 0;                                                  
    }
    
    cudaSetDevice(devID);
    printf("gpuDeviceInit() CUDA Device [%d]: \"%s\n", devID, deviceProp.name);

    return 1;
}

inline int _ConvertSMVer2Cores(int major, int minor)
{
	// Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
	typedef struct {
		int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] = 
	{ { 0x10,  8 },
	  { 0x11,  8 },
	  { 0x12,  8 },
	  { 0x13,  8 },
	  { 0x20, 32 },
	  { 0x21, 48 },
	  {   -1, -1 } 
	};

	int index = 0;
	while (nGpuArchCoresPerSM[index].SM != -1) {
		if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor) ) {
			return nGpuArchCoresPerSM[index].Cores;
		}
		index++;
	}
	printf("MapSMtoCores undefined SMversion %d.%d!\n", major, minor);
	return -1;
}

// This function returns the best GPU (with maximum GFLOPS)
int gpuGetMaxGflopsDeviceId()
{
    int current_device     = 0, sm_per_multiproc  = 0;
    int max_compute_perf   = 0, max_perf_device   = 0;
    int device_count       = 0, best_SM_arch      = 0;
    cudaDeviceProp deviceProp;
    cudaGetDeviceCount( &device_count );
    
    // Find the best major SM Architecture GPU device
    while (current_device < device_count)
    {
        cudaGetDeviceProperties( &deviceProp, current_device );
        if (deviceProp.major > 0 && deviceProp.major < 9999)
        {
            best_SM_arch = MAX(best_SM_arch, deviceProp.major);
        }
        current_device++;
    }

    // Find the best CUDA capable GPU device
    current_device = 0;
    while( current_device < device_count )
    {
        cudaGetDeviceProperties( &deviceProp, current_device );
        if (deviceProp.major == 9999 && deviceProp.minor == 9999)
        {
            sm_per_multiproc = 1;
        }
        else
        {
            sm_per_multiproc = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor);
        }
        
        int compute_perf  = deviceProp.multiProcessorCount * sm_per_multiproc * deviceProp.clockRate;
        
    if( compute_perf  > max_compute_perf )
    {
            // If we find GPU with SM major > 2, search only these
            if ( best_SM_arch > 2 )
            {
                // If our device==dest_SM_arch, choose this, or else pass
                if (deviceProp.major == best_SM_arch)
                {
                    max_compute_perf  = compute_perf;
                    max_perf_device   = current_device;
                 }
            }
            else
            {
                max_compute_perf  = compute_perf;
                max_perf_device   = current_device;
             }
        }
        ++current_device;
    }
    return max_perf_device;
}


// Initialization code to find the best CUDA Device
int findCudaDevice()
{
    cudaDeviceProp deviceProp;
    int devID = 0;
    
    // Escolhe o device com maior taxa de Gflops/s
    devID = gpuGetMaxGflopsDeviceId();
    cudaSetDevice( devID );
    cudaGetDeviceProperties(&deviceProp, devID);
    printf("GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID, deviceProp.name, deviceProp.major, deviceProp.minor);
    
    return devID;
}
// end of CUDA Helper Functions
