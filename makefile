CC = nvcc
CFLAGS = -O0 -lm `pkg-config glib-2.0 --cflags --libs`
OPENMP = -fopenmp

INSTALL = ../bin

shunter-cmd:shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o
	$(CC) -G shunter-cmd.o log.o load_data.o aux.o operacoes.o busca.o $(CFLAGS) -o shunter-cmd
	
shunter-cmd.o:shunter-cmd.cu
	$(CC) $(CFLAGS) -G -g -c shunter-cmd.cu	
	
log.o:log.cu
	$(CC) -G -g -c log.cu
	
load_data.o:load_data.cu
	$(CC) -G -g -c load_data.cu

aux.o:aux.cu
	$(CC) -G -g -c aux.cu

operacoes.o:operacoes.cu
	$(CC) $(CFLAGS) -G -g -c operacoes.cu

busca.o:busca.cu
	$(CC) -G -g -c busca.cu
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd

install:
	cp shunter-cmd $(INSTALL)
