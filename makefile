CC = nvcc
CFLAGS = -O0 -lm `pkg-config glib-2.0 --cflags --libs`
OPENMP = -fopenmp

INSTALL = ../bin

shunter-cmd:shunter-cmd.o log.o load_data.o aux.o operacoes.o
	$(CC) shunter-cmd.o log.o load_data.o aux.o operacoes.o $(CFLAGS) -o shunter-cmd
	
shunter-cmd.o:shunter-cmd.cu
	$(CC) $(CFLAGS) -G -g -c shunter-cmd.cu	
	
log.o:log.cu
	$(CC) -g -c log.cu
	
load_data.o:load_data.cu
	$(CC) -g -c load_data.cu

aux.o:aux.cu
	$(CC) -g -c aux.cu

operacoes.o:operacoes.cu
	$(CC) $(CFLAGS) -g -c operacoes.cu
	
clean:
	rm -f *.o *.{c,h}~ shunter-cmd shunter-gui

install:
	cp shunter-* $(INSTALL)
