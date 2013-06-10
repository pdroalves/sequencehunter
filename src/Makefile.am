CUDA_CC = nvcc
CC = colorgcc
CPP = g++
GLIB_CFLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

GIO_CFLAGS = `pkg-config --cflags gio-2.0`
GIO_LIBS = `pkg-config --libs gio-2.0`

OPENMP = -fopenmp
OPENMP_CUDA = -Xcompiler $(OPENMP)

CUDA = -L/usr/local/cuda/lib64 -lcudart 
CUDA_ARCH = 

CLI_NAME = shunter-cli
GUI_NAME = shunter-gui.jar

SQLITE3 = -lsqlite3

INSTALL = /usr/local/bin
OBJ = /tmp/
BIN = ./
SOURCE = ./
JAVA_SOURCE = $(SOURCE)Java\ Src/

OPT = -Wall
OPT_CUDA = -Xcompiler -Wall

all:build

c:build
java:build_gui

#########################################
#########################################
#########################################
#########################################

compile:shunter-cmd.o log.o load_data.o go_hunter.o go_hunter_cuda.o go_hunter_noncuda.o operacoes.o busca.o fila.o cuda_functions.o database_manager.o socket.o database.o nc_busca.o build_control	
	echo "CLI Compiled."
	
build:shunter-cmd.o log.o load_data.o go_hunter.o go_hunter_cuda.o go_hunter_noncuda.o operacoes.o busca.o fila.o cuda_functions.o database_manager.o socket.o database.o nc_busca.o build_control	
	$(BIN)build_control $(SOURCE)Headers/version
	$(CUDA_CC) -G -g $(OPT_CUDA) -o $(BIN)$(CLI_NAME) $(OBJ)shunter-cmd.o $(OBJ)log.o $(OBJ)load_data.o $(OBJ)go_hunter.o $(OBJ)go_hunter_cuda.o $(OBJ)go_hunter_noncuda.o $(OBJ)operacoes.o $(OBJ)busca.o $(OBJ)fila.o $(OBJ)cuda_functions.o $(OBJ)database_manager.o $(OBJ)nc_busca.o $(OBJ)socket.o $(OBJ)database.o $(GLIB_LIBS) $(GIO_LIBS) $(OPENMP_CUDA) $(SQLITE3)
	echo "CLI built"	
	

#########################################
############ GCC ######## ###############
#########################################
	
shunter-cmd.o:$(SOURCE)shunter-cmd.c
	$(CC) -Wall  -g -c  $(SOURCE)shunter-cmd.c -o $(OBJ)shunter-cmd.o $(GLIB_CFLAGS) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lcuda -lcudart
	
go_hunter.o:$(SOURCE)Go_Hunter/go_hunter.c
	$(CC) -Wall   -g -c $(SOURCE)Go_Hunter/go_hunter.c -o $(OBJ)go_hunter.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
go_hunter_cuda.o:$(SOURCE)Go_Hunter/go_hunter_cuda.c
	$(CC) -Wall   -g -c $(SOURCE)Go_Hunter/go_hunter_cuda.c -o $(OBJ)go_hunter_cuda.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
go_hunter_noncuda.o:$(SOURCE)Go_Hunter/go_hunter_noncuda.c
	$(CC) -Wall  -g -c  $(SOURCE)Go_Hunter/go_hunter_noncuda.c -o $(OBJ)go_hunter_noncuda.o $(GLIB_CFLAGS) $(OPENMP) -L/usr/local/cuda/lib64 -I/usr/local/cuda/include
	
load_data.o:$(SOURCE)External/load_data.c
	$(CC) -Wall  -g -c  $(SOURCE)External/load_data.c -o $(OBJ)load_data.o $(GLIB_CFLAGS)  -L/usr/local/cuda/lib64 -I/usr/local/cuda/include -lstdc++
	
database_manager.o:$(SOURCE)Processing/database_manager.c
	$(CC) -Wall  -g -c $(SOURCE)Processing/database_manager.c -o $(OBJ)database_manager.o $(GLIB_CFLAGS) $(GIO_CFLAGS)
	
socket.o:$(SOURCE)External/socket.c
	$(CC) -Wall  -g -c $(SOURCE)External/socket.c -o $(OBJ)socket.o $(GLIB_CFLAGS) $(GIO_CFLAGS)
	
operacoes.o:$(SOURCE)Assist/operacoes.c
	$(CC) -Wall  -g -c $(SOURCE)Assist/operacoes.c -o $(OBJ)operacoes.o $(GLIB_CFLAGS) $(OPENMP) 
	
log.o:$(SOURCE)Assist/log.c
	$(CC) -Wall  -g -c $(SOURCE)Assist/log.c -o $(OBJ)log.o $(GLIB_CFLAGS)
	
fila.o:$(SOURCE)Processing/fila.c
	$(CC) -Wall  -g -c $(SOURCE)Processing/fila.c -o $(OBJ)fila.o $(GLIB_CFLAGS)
	
database.o:$(SOURCE)Processing/database.c
	$(CC) -Wall  -g -c $(SOURCE)Processing/database.c -o $(OBJ)database.o $(SQLITE3)  $(OPENMP)

nc_busca.o:$(SOURCE)Search/nc_busca.c
	$(CC) -Wall  -g -c  $(SOURCE)Search/nc_busca.c -o $(OBJ)nc_busca.o

#########################################
############ NVCC LINUX##################
#########################################
busca.o:$(SOURCE)Search/busca.cu
	$(CUDA_CC) -Xptxas -v $(CUDA_ARCH) -G -g -c $(SOURCE)Search/busca.cu -o $(OBJ)busca.o
	
cuda_functions.o:$(SOURCE)Assist/cuda_functions.cu
	$(CUDA_CC) $(CUDA_ARCH) -G -g -c $(SOURCE)Assist/cuda_functions.cu -o $(OBJ)cuda_functions.o $(GLIB_CFLAGS)

build_control:$(SOURCE)Assist/build_control.c
	gcc $(SOURCE)Assist/build_control.c -o $(BIN)build_control
	
clean:
	rm -f $(OBJ)*.o $(BIN)$(CLI_NAME) $(BIN)$(GUI_NAME)
	echo "It's clean"

install:
	sudo cp $(BIN)$(CLI_NAME) $(BIN)$(GUI_NAME) $(INSTALL)
	echo "Done"
