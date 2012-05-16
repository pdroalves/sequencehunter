C = gcc
CC = nvcc
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
GTK_LIBS = `pkg-config --libs gtk+-2.0`

OPENMP = -fopenmp
OPENMP_CUDA = -Xcompiler $(OPENMP)

CUDA = -L/usr/local/cuda/lib64 -lcudart -lcuda

INSTALL = ../../bin

all:cmd gui

cmd:shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o
	$(C) -g shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o $(GLIB_CFLAGS) $(GLIB_LIBS) -o shunter-cmd $(OPENMP) $(CUDA)
	
gui:shunter-gui.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o
	$(C) -g shunter-gui.o log.o load_data.o aux.o operacoes.o busca.o pilha.o processing_data.o linkedlist.o -lm -O0 $(GLIB_LIBS) $(GLIB_LIBS) $(GTK_CFLAGS) $(GTK_LIBS) -o shunter-gui $(OPENMP) $(CUDA)
	
shunter-cmd.o:shunter-cmd.cu
	$(CC) $(GLIB_CFLAGS) -arch=sm_20 -G -g -c shunter-cmd.cu	$(CUDA)

shunter-gui.o:shunter-gui.c
	$(C) -Wall -c shunter-gui.c -o shunter-gui.o $(GTK_CFLAGS) 
	
log.o:log.cu
	$(CC) -G -g -c log.cu
	
load_data.o:load_data.cu
	$(CC) -G -g -c load_data.cu

aux.o:aux.cu
	$(CC) -arch=sm_20 $(OPENMP_CUDA) -G -g -c aux.cu

operacoes.o:operacoes.cu
	$(CC) -arch=sm_20 $(GLIB_CFLAGS) -g -G -c operacoes.cu

busca.o:busca.cu
	$(CC) -arch=sm_20 -G -g -c busca.cu
	
pilha.o:pilha.cu
	$(CC) -arch=sm_20 -G -g -c pilha.cu
	
linkedlist.o:linkedlist.cu
	$(CC) -G -g -c linkedlist.cu
	
processing_data.o:processing_data.cu
	$(CC) -G -g -c processing_data.cu
	
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd

install:
	cp shunter-cmd $(INSTALL)
