
#ifndef CUDA_FUNCTIONS_H
#define CUDA_FUNCTIONS_H
int gpuDeviceInit(int devID);
int _ConvertSMVer2Cores(int major, int minor);
int gpuGetMaxGflopsDeviceId();
int findCudaDevice();
int check_gpu_mode();
int escolhe_GPU();
void copyStrToDevice(char *src,char *dst,int size);
#endif
