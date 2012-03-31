CC = nvcc
CFLAGS = -O0 -lm `pkg-config glib-2.0 --cflags --libs`
OPENMP = -Xcompiler -fopenmp

INSTALL = ../bin

shunter-cmd:shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o cuda_stack.o pilha.o
	$(CC) -arch=sm_20 -G shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o cuda_stack.o pilha.o $(CFLAGS) -o shunter-cmd $(OPENMP)
	
shunter-cmd.o:shunter-cmd.cu
	$(CC) $(CFLAGS) -G -g -c shunter-cmd.cu	
	
log.o:log.cu
	$(CC) -G -g -c log.cu
	
load_data.o:load_data.cu
	$(CC) -G -g -c load_data.cu

aux.o:aux.cu
	$(CC) $(OPENMP) -G -g $(OPENMP) -c aux.cu

operacoes.o:operacoes.cu
	$(CC) $(CFLAGS) -G -g -c operacoes.cu

busca.o:busca.cu
	$(CC) -arch=sm_20 -G -g -c busca.cu
	
cuda_stack.o:cuda_stack.cu
	$(CC) -arch=sm_20 -G -g -c cuda_stack.cu
	
pilha.o:pilha.cu
	$(CC) -arch=sm_20 -G -g -c pilha.cu
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd

install:
	cp shunter-cmd $(INSTALL)
