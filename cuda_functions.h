int gpuDeviceInit(int devID);
inline int _ConvertSMVer2Cores(int major, int minor);
int gpuGetMaxGflopsDeviceId();
int findCudaDevice();
int check_gpu_mode();
int escolhe_GPU();
void getDevice(int deviceCount,struct cudaDeviceProp deviceProp,gboolean verbose);

