CUDA_CC = nvcc
CC = gcc
CPP = g++
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

GIO_CFLAGS = `pkg-config --cflags gio-2.0`
GIO_LIBS = `pkg-config --libs gio-2.0`

OPENMP = -fopenmp
OPENMP_CUDA = -Xcompiler $(OPENMP)

CUDA = -L/usr/local/cuda/lib64 -lcudart 
CUDA_ARCH = -arch=sm_21

CLI_NAME = shunter-cli
GUI_NAME = shunter-gui.jar
DB_MANAGER_NAME = database-manager.jar

HAMSTERDB_LIB = /usr/local/lib/libhamsterdb.a

INSTALL = /usr/local/bin
OBJ = obj/
BIN = bin/
SOURCE = src/
JAVA_SOURCE = $(SOURCE)Java\ Src/

all:cmd gui

#########################################
##Executaveis############################
#########################################

##Linux##################################
#########################################
cmd:$(OBJ)shunter-cmd.o $(OBJ)log.o $(OBJ)load_data.o $(OBJ)go_hunter.o $(OBJ)go_hunter_cuda.o $(OBJ)go_hunter_noncuda.o $(OBJ)operacoes.o $(OBJ)busca.o $(OBJ)fila.o $(OBJ)processing_data.o $(OBJ)linkedlist.o $(OBJ)cuda_functions.o $(OBJ)hashtable.o $(OBJ)socket.o $(OBJ)database.o build_control
	$(BIN)build_control $(SOURCE)Headers/version
	$(CUDA_CC) -G -g -o $(BIN)$(CLI_NAME) $(OBJ)shunter-cmd.o $(OBJ)log.o $(OBJ)load_data.o $(OBJ)go_hunter.o $(OBJ)go_hunter_cuda.o $(OBJ)go_hunter_noncuda.o $(OBJ)operacoes.o $(OBJ)busca.o $(OBJ)fila.o $(OBJ)processing_data.o $(OBJ)linkedlist.o $(OBJ)cuda_functions.o $(OBJ)hashtable.o $(OBJ)socket.o $(OBJ)database.o $(GLIB_LIBS) $(GIO_LIBS) $(OPENMP_CUDA) $(HAMSTERDB_LIB)
	echo "CLI built"	
	
gui:$(JAVA_SOURCE)Sequence\ Hunter\ GUI/makefile
	make -C $(JAVA_SOURCE)Sequence\ Hunter\ GUI/
	cp $(JAVA_SOURCE)Sequence\ Hunter\ GUI/$(GUI_NAME) $(BIN)
	echo "Gui built"

#########################################
############ GCC e MingWW ###############
#########################################
	
$(OBJ)shunter-cmd.o:$(SOURCE)shunter-cmd.c
	$(CC) -g -c $(SOURCE)shunter-cmd.c -o $(OBJ)shunter-cmd.o $(GLIB_CFLAGS) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lcuda -lcudart
	
$(OBJ)go_hunter.o:$(SOURCE)Go_Hunter/go_hunter.c
	$(CC) -g -c $(SOURCE)Go_Hunter/go_hunter.c -o $(OBJ)go_hunter.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
$(OBJ)go_hunter_cuda.o:$(SOURCE)Go_Hunter/go_hunter_cuda.c
	$(CC) -g -c $(SOURCE)Go_Hunter/go_hunter_cuda.c -o $(OBJ)go_hunter_cuda.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
$(OBJ)go_hunter_noncuda.o:$(SOURCE)Go_Hunter/go_hunter_noncuda.c
	$(CC) -g -c $(SOURCE)Go_Hunter/go_hunter_noncuda.c -o $(OBJ)go_hunter_noncuda.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
$(OBJ)load_data.o:$(SOURCE)External/load_data.c
	$(CC) -g -c $(SOURCE)External/load_data.c -o $(OBJ)load_data.o $(GLIB_CFLAGS)  -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++
	
$(OBJ)hashtable.o:$(SOURCE)External/hashtable.c
	$(CC) -g -c $(SOURCE)External/hashtable.c -o $(OBJ)hashtable.o $(GLIB_CFLAGS) $(GIO_CFLAGS) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
$(OBJ)socket.o:$(SOURCE)External/socket.c
	$(CC) -g -c $(SOURCE)External/socket.c -o $(OBJ)socket.o $(GLIB_CFLAGS) $(GIO_CFLAGS)
	
$(OBJ)operacoes.o:$(SOURCE)Assist/operacoes.c
	$(CC) -g -c $(SOURCE)Assist/operacoes.c -o $(OBJ)operacoes.o $(GLIB_CFLAGS) $(OPENMP) 
	
$(OBJ)log.o:$(SOURCE)Assist/log.c
	$(CC) -g -c $(SOURCE)Assist/log.c -o $(OBJ)log.o $(GLIB_CFLAGS)
	
$(OBJ)linkedlist.o:$(SOURCE)Processing/linkedlist.c
	$(CC) -g -c $(SOURCE)Processing/linkedlist.c -o $(OBJ)linkedlist.o $(GLIB_CFLAGS)
	
$(OBJ)processing_data.o:$(SOURCE)Processing/processing_data.c
	$(CC) -g -c $(SOURCE)Processing/processing_data.c -o $(OBJ)processing_data.o $(GLIB_CFLAGS) 
	
$(OBJ)fila.o:$(SOURCE)Processing/fila.c
	$(CC) -g -c $(SOURCE)Processing/fila.c -o $(OBJ)fila.o $(GLIB_CFLAGS)
	
$(OBJ)database.o:$(SOURCE)Processing/database.c
	$(CPP) -g -c $(SOURCE)Processing/database.c -o $(OBJ)database.o -lhamsterdb -I/usr/local/cuda/include $(CUDA) $(OPENMP)
	
#########################################
############ NVCC LINUX##################
#########################################
$(OBJ)busca.o:$(SOURCE)Search/busca.cu
	$(CUDA_CC) -Xptxas -v $(CUDA_ARCH) -G -g -c $(SOURCE)Search/busca.cu -o $(OBJ)busca.o
	
$(OBJ)cuda_functions.o:$(SOURCE)Assist/cuda_functions.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c $(SOURCE)Assist/cuda_functions.cu -o $(OBJ)cuda_functions.o $(GLIB_CFLAGS)

build_control:$(SOURCE)Assist/build_control.c
	gcc $(SOURCE)Assist/build_control.c -o $(BIN)build_control
	
clean:
	rm -f $(OBJ)*.o *.{c,h}~ $(BIN)$(CLI_NAME) $(BIN)$(GUI_NAME)
	make -C $(SOURCE)Gui/ clean
	echo "It's clean"

install:
	#sudo rm $(INSTALL)/$(CLI_NAME) $(INSTALL)/$(GUI_NAME)
	sudo cp $(BIN)$(CLI_NAME) $(BIN)$(GUI_NAME) $(INSTALL)
	echo "Done"
