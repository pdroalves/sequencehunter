CC_LINUX = gcc
CC_WIN64 = x86_64-w64-mingw32-gcc.exe
CUDA_CC = nvcc
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
GTK_LIBS = `pkg-config --libs gtk+-2.0`

OPENMP = -fopenmp
OPENMP_CUDA = -Xcompiler $(OPENMP)

CUDA = -L/usr/local/cuda/lib64 -lcudart -lcuda

INSTALL = ../../bin

linux:cmd_linux gui_linux

windows:cmd_win gui_win

#########################################
##Executaveis############################
#########################################

##Linux##################################
#########################################
cmd_linux:shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o
	$(CC_LINUX) -g shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o $(GLIB_CFLAGS) $(GLIB_LIBS) -o shunter-cmd $(OPENMP) $(CUDA)
	
gui_linux:shunter-gui.o_linux log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o
	$(CC_LINUX) -g shunter-gui.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o -lm -O0 $(GLIB_LIBS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) -o shunter-gui $(OPENMP) $(CUDA)

#########################################
#Windows#################################
#########################################
cmd_win:shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o
	$(CC_WIN64) -g shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o $(GLIB_CFLAGS) $(GLIB_LIBS) -o shunter-cmd $(OPENMP) $(CUDA)
	
gui_win:shunter-gui.o_win log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o
	$(CC_WIN64) -g shunter-gui.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o -lm -O0 $(GLIB_LIBS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) -o shunter-gui $(OPENMP) $(CUDA)

#########################################
#Mac#####################################
#########################################

#########################################
############ GCC e MingWW ###############
#########################################

shunter-gui.o_linux:shunter-gui.c
	$(CC_LINUX) -Wall -c shunter-gui.c -o shunter-gui.o $(GTK_CFLAGS) 
	
shunter-gui.o_win:shunter-gui.c
	$(CC_WIN64) -Wall -c shunter-gui.c -o shunter-gui.o $(GTK_CFLAGS) 
	
#########################################
############ NVCC #######################
#########################################

shunter-cmd.o:shunter-cmd.cu
	$(CUDA_CC) $(GLIB_CFLAGS) -arch=sm_20 -G -g -c shunter-cmd.cu	$(CUDA)
	
log.o:log.cu
	$(CUDA_CC) -G -g -c log.cu
	
load_data.o:load_data.cu
	$(CUDA_CC) -G -g -c load_data.cu

aux.o:aux.cu
	$(CUDA_CC) -arch=sm_20 $(OPENMP_CUDA) -G -g -c aux.cu

operacoes.o:operacoes.cu
	$(CUDA_CC) -arch=sm_20 $(GLIB_CFLAGS) -g -G -c operacoes.cu

busca.o:busca.cu
	$(CUDA_CC) -arch=sm_20 -G -g -c busca.cu
	
pilha.o:pilha.cu
	$(CUDA_CC) -arch=sm_20 -G -g -c pilha.cu
	
linkedlist.o:linkedlist.cu
	$(CUDA_CC) -G -g -c linkedlist.cu
	
processing_data.o:processing_data.cu
	$(CUDA_CC) -G -g -c processing_data.cu
	

#########################################
#########################################
#########################################
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd shunter-gui shunter-cmd.exe shunter-gui.exe

install:
	cp shunter-cmd shunter-gui shunter-cmd.exe shunter-gui.exe $(INSTALL)
