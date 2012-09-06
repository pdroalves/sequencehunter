#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime_api.h>

extern "C" int gpuDeviceInit(int devID);
extern "C" inline int _ConvertSMVer2Cores(int major, int minor);
extern "C" int gpuGetMaxGflopsDeviceId();
extern "C" int findCudaDevice();

 int MAX(int A,int B){
	return A >= B ? A:B;
 }

extern "C" int check_gpu_mode(){
	
	return gpuDeviceInit(findCudaDevice());
}
extern "C" int escolhe_GPU(){
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

extern "C" void getDevice(int deviceCount,cudaDeviceProp deviceProp,bool verbose){

	  int i;
	  int device;
	  cudaError_t erro;

		 printf("Dispositivos encontrados:\n");
		  for(i=0;i<deviceCount;i++)
			{
			  erro = cudaGetDeviceProperties(&deviceProp,i);
				if(erro != cudaSuccess) exit(1);
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
		if(erro != cudaSuccess) exit(1);
		
		//Configura qual dispositivo deve ser usado
		erro = cudaSetDevice(device);
		if(erro != cudaSuccess) exit(1);
			
		//O dispositivo foi escolhido
		erro = cudaGetDeviceProperties(&deviceProp,device);
		if(erro != cudaSuccess) exit(1);
		
		if(verbose) printf("Dispositivo configurado para uso: %s\n\n",deviceProp.name); 
	 
	}
	
	
extern "C" void copyStrToDevice(char *src,char *dst,int size){
	cudaMemcpy(src,dst,size,cudaMemcpyHostToDevice);
	return;
}
	
	//#######################
	
	// General GPU Device CUDA Initialization
extern "C" int gpuDeviceInit(int devID)
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

extern "C" inline int _ConvertSMVer2Cores(int major, int minor)
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
extern "C" int gpuGetMaxGflopsDeviceId()
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
extern "C" int findCudaDevice()
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

