CUDA_CC = nvcc

#LINUX
CC_LINUX = gcc
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
GTK_LIBS = `pkg-config --libs gtk+-2.0`

OPENMP = -fopenmp
OPENMP_CUDA = -Xcompiler $(OPENMP)

CUDA = -L/usr/local/cuda/lib64 -lcudart 
CUDA_ARCH = -arch=sm_20

#WIN
CUDA_CC_WIN = nvcc --compiler-binddir /opt/cross_win64/bin/x86_64-w64-mingw32-gcc
CC_WIN64 = x86_64-w64-mingw32-gcc -O0 -Wall -mms-bitfields -mwindows
WIN32_BUILD = bin/win32
PKG_CONFIG_PATH = ./w32/lib/pkgconfig

OPENMP_WIN = -I/opt/cross_win64/lib/gcc/x86_64-w64-mingw32/4.5.4/include

GTK_CFLAGS_WIN := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) \
          pkg-config --cflags gtk+-win32-2.0) -I/home/pedro/Dropbox/Estagio/LNBio/Codigos/Sequence\ Generator/src/pthreads-win/include
GTK_LIBS_WIN := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) \
          pkg-config --libs gtk+-win32-2.0)
          
GLIB_CFLAGS_WIN := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) \
          pkg-config --cflags glib-2.0)
GLIB_LIBS_WIN := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) \
          pkg-config --libs glib-2.0)          

CFLAGS_WIN = $(GLIB_CFLAGS_WIN) $(GTK_CFLAGS_WIN)
LIBS_WIN = $(GLIB_LIBS_WIN) $(GTK_LIBS_WIN)

INSTALL = ../../bin

all:windows linux

linux:cmd_linux gui_linux

windows:cmd_win gui_win

#########################################
##Executaveis############################
#########################################

##Linux##################################
#########################################
cmd_linux:shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o cuda_functions.o
	$(CUDA_CC) -G -g -o shunter-cmd shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o cuda_functions.o $(GLIB_CFLAGS) $(GLIB_LIBS) $(OPENMP_CUDA) -Xcompiler --Wall
	
gui_linux:shunter-gui.o_linux log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o cuda_functions.o
	$(CC_LINUX) -g shunter-gui.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o cuda_functions.o -lm -O0 $(GLIB_LIBS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) -o shunter-gui $(OPENMP) $(CUDA) -lstdc++ 

#########################################
#Windows#################################
#########################################
cmd_win:shunter-cmd.o_win log.o_win load_data.o_win aux.o_win operacoes.o_win busca.o_win pilha.o_win processing_data.o_win linkedlist.o_win cuda_functions.o_win
	$(CUDA_CC) -o shunter-cmd.exe shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o cuda_functions.o -lm -O0 $(GLIB_CFLAGS) $(GLIB_LIBS) $(OPENMP_WIN)

gui_win:shunter-gui.o_win log.o_win load_data.o_win aux.o_win operacoes.o_win busca.o_win pilha.o_win processing_data.o_win linkedlist.o_win cuda_functions.o_win
	$(CC_WIN) -g shunter-gui.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o -lm -O0 $(GLIB_LIBS_WIN) $(GLIB_LIBS_WIN) $(GTK_CFLAGS_WIN) $(GTK_LIBS_WIN) -o shunter-gui.exe $(OPENMP) $(CUDA) 

#########################################
#Mac#####################################
#########################################

#########################################
############ GCC e MingWW ###############
#########################################

#LINUX

shunter-gui.o_linux:shunter-gui.c
	$(CC_LINUX) -g -c shunter-gui.c -o shunter-gui.o $(GTK_CFLAGS)  -L/usr/local/cuda/lib64
	
shunter-cmd.o:shunter-cmd.c
	$(CC_LINUX) -g -c shunter-cmd.c -o shunter-cmd.o $(GLIB_CFLAGS) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lcuda -lcudart
	
aux.o:aux.c
	$(CC_LINUX) -g -c aux.c -o aux.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
load_data.o:load_data.c
	$(CC_LINUX) -g -c load_data.c -o load_data.o $(GLIB_CFLAGS)  -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++
	
operacoes.o:operacoes.c
	$(CC_LINUX) -g -c operacoes.c -o operacoes.o $(GLIB_CFLAGS) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++ -lcuda -lcudart
	
log.o:log.c
	$(CC_LINUX) -g -c log.c -o log.o
	
linkedlist.o:linkedlist.c
	$(CC_LINUX) -g -c linkedlist.c -o linkedlist.o
	
processing_data.o:processing_data.c
	$(CC_LINUX) -g -c processing_data.c -o processing_data.o
	
pilha.o:pilha.c
	$(CC_LINUX) -g -c pilha.c -o pilha.o

#WIN
shunter-gui.o_win:shunter-gui.c
	$(CC_WIN64) -g -c shunter-gui.c -o shunter-gui.o $(GTK_CFLAGS_WIN) 

shunter-cmd.o_win:shunter-cmd.c
	$(CC_WIN64) -g -c shunter-cmd.c -o shunter-cmd.o $(GLIB_CFLAGS_WIN) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lcuda -lcudart
	
aux.o_win:aux.c
	$(CC_WIN64) -g -c aux.c -o aux.o $(GLIB_CFLAGS_WIN) $(OPENMP_WIN) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
load_data.o_win:load_data.c
	$(CC_WIN64) -g -c load_data.c -o load_data.o $(GLIB_CFLAGS_WIN)  -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++
	
operacoes.o_win:operacoes.c
	$(CC_WIN64) -g -c operacoes.c -o operacoes.o $(GLIB_CFLAGS_WIN) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++ -lcuda -lcudart
	
log.o_win:log.c
	$(CC_WIN64) -g -c log.c -o log.o
	
linkedlist.o_win:linkedlist.c
	$(CC_WIN64) -g -c linkedlist.c -o linkedlist.o $(OPENMP_WIN)
	
processing_data.o_win:processing_data.c
	$(CC_WIN64) -g -c processing_data.c -o processing_data.o
	
pilha.o_win:pilha.c
	$(CC_WIN64) -g -c pilha.c -o pilha.o
	
#########################################
############ NVCC LINUX##################
#########################################
busca.o:busca.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c busca.cu
	
cuda_functions.o:cuda_functions.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c cuda_functions.cu $(GLIB_CFLAGS)	
#########################################
############ NVCC WIN ###################
#########################################
busca.o_win:busca.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c busca.cu


cuda_functions.o_win:cuda_functions.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c cuda_functions.cu $(GLIB_CFLAGS)
#########################################
#########################################
#########################################
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd shunter-gui shunter-cmd.exe shunter-gui.exe

install:
	cp shunter-cmd shunter-gui shunter-cmd.exe shunter-gui.exe $(INSTALL)
