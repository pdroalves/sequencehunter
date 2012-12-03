CUDA_CC = nvcc
CC_LINUX = gcc
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
GTK_LIBS = `pkg-config --libs gtk+-2.0`

OPENMP = -fopenmp
OPENMP_CUDA = -Xcompiler $(OPENMP)

CUDA = -L/usr/local/cuda/lib64 -lcudart 
CUDA_ARCH = -arch=sm_20 --maxrregcount=20

INSTALL = /home/pedro/Dropbox/Estagio/LNBio/Codigos/bin

all:cmd gui

#########################################
##Executaveis############################
#########################################

##Linux##################################
#########################################
cmd:shunter-cmd.o log.o load_data.o go_hunter.o go_hunter_cuda.o go_hunter_noncuda.o operacoes.o busca.o fila.o processing_data.o linkedlist.o cuda_functions.o ghashtable.o build_control
	./build_control version
	$(CUDA_CC) -G -g -o shunter-cmd shunter-cmd.o log.o load_data.o go_hunter.o go_hunter_cuda.o go_hunter_noncuda.o operacoes.o busca.o fila.o processing_data.o linkedlist.o cuda_functions.o ghashtable.o $(GLIB_CFLAGS) $(GLIB_LIBS) $(OPENMP_CUDA)
	
gui:shunter-gui.o_linux log.o cuda_functions.o load_data.o log.o processing_data.o ghashtable.o operacoes.o linkedlist.o build_control
	$(CC_LINUX) -g shunter-gui.o cuda_functions.o load_data.o log.o processing_data.o ghashtable.o operacoes.o linkedlist.o -lm -O0 $(GLIB_LIBS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) -o shunter-gui $(OPENMP) $(CUDA)

#########################################
############ GCC e MingWW ###############
#########################################

#LINUX

shunter-gui.o_linux:shunter-gui.c
	$(CC_LINUX) -g -c shunter-gui.c -o shunter-gui.o $(GTK_CFLAGS)  -L/usr/local/cuda/lib64
	
shunter-cmd.o:shunter-cmd.c
	$(CC_LINUX) -g -c shunter-cmd.c -o shunter-cmd.o $(GLIB_CFLAGS) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lcuda -lcudart
	
go_hunter.o:go_hunter.c
	$(CC_LINUX) -g -c go_hunter.c -o go_hunter.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
go_hunter_cuda.o:go_hunter_cuda.c
	$(CC_LINUX) -g -c go_hunter_cuda.c -o go_hunter_cuda.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
go_hunter_noncuda.o:go_hunter_noncuda.c
	$(CC_LINUX) -g -c go_hunter_noncuda.c -o go_hunter_noncuda.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
load_data.o:load_data.c
	$(CC_LINUX) -g -c load_data.c -o load_data.o $(GLIB_CFLAGS)  -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++
	
operacoes.o:operacoes.c
	$(CC_LINUX) -g -c operacoes.c -o operacoes.o $(GLIB_CFLAGS) $(OPENMP) 
	
log.o:log.c
	$(CC_LINUX) -g -c log.c -o log.o $(GLIB_CFLAGS)
	
linkedlist.o:linkedlist.c
	$(CC_LINUX) -g -c linkedlist.c -o linkedlist.o $(GLIB_CFLAGS)
	
processing_data.o:processing_data.c
	$(CC_LINUX) -g -c processing_data.c -o processing_data.o $(GLIB_CFLAGS) 
	
fila.o:fila.c
	$(CC_LINUX) -g -c fila.c -o fila.o $(GLIB_CFLAGS)
	
ghashtable.o:ghashtable.c
	$(CC_LINUX) -g -c ghashtable.c -o ghashtable.o $(GLIB_CFLAGS)

	
#########################################
############ NVCC LINUX##################
#########################################
busca.o:busca.cu
	$(CUDA_CC) -Xptxas -v $(CUDA_ARCH) -G -g -c busca.cu
	
cuda_functions.o:cuda_functions.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c cuda_functions.cu $(GLIB_CFLAGS)
	
	

build_control:build_control.c
	gcc build_control.c -o build_control
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd shunter-gui shunter-cmd.exe shunter-gui.exe

install:
	cp shunter-cmd $(INSTALL)
