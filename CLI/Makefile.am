include cuda.mk

shunterCuda_LINK = $(CC) $(shunterCuda_CFLAGS) $(CFLAGS) \
$(shunterCuda_LDFLAGS) $(LDFLAGS) -o $@

bin_PROGRAMS	=shunter
shunter_SOURCES	= src/shunter-cmd.c src/Go_Hunter/go_hunter.c  src/Go_Hunter/go_hunter_cuda.c src/Go_Hunter/go_hunter_noncuda.c src/External/load_data.c src/Processing/database_manager.c src/External/socket.c src/Assist/operacoes.c src/Assist/log.c src/Processing/fila.c src/Processing/database.c src/Search/nc_busca.c src/Search/busca.cu src/Assist/cuda_functions.cu

shunter_CFLAGS = $(CUDA_CFLAGS) $(GLIB_CFLAGS) $(GIO_CFLAGS) $(SQLITE3_CFLAGS) @OPENMP_CFLAGS@
shunter_LDADD = $(CUDA_LIBS) $(GLIB_LIBS) $(GIO_LIBS)  $(SQLITE3_LIBS) @OPENMP_CFLAGS@ @LIBS@
shunter_LDFLAGS = $(CUDA_LDFLAGS) $(GLIB_LDFLAGS) $(GIO_LDFLAGS) $(SQLITE3_LDFLAGS)
