#include <stdio.h>
#include <omp.h> 
#include <glib.h>
#include <string.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#ifdef _WIN32
#include <Windows.h>
#define SLEEP(a) Sleep(1000*a)
#else
#define SLEEP(a) sleep(a)
#endif
#include "../Headers/database.h"
#include "../Headers/database_manager.h"
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"
#include "../Headers/operacoes.h"
#include "../Headers/nc_busca.h"
#include "../Headers/log.h"
#include "../Headers/fila.h"
#include "../Headers/nc_busca.h"
#include "../Headers/go_hunter.h"
#include "../Headers/socket.h"
#include "sqlite3.h"
#include <time.h>

// Lista de threads a serem criados
enum threads { 
  THREAD_SEARCH,
  THREAD_QUEUE,
  THREAD_DATABASE,
  OMP_NTHREADS
};
#define buffer_size 512 // Capacidade máxima do buffer
#define LOADER_QUEUE_MAX_SIZE 1e6
#define GUI_SOCKET_PORT 9332
#define GIGA 1073741824 

gboolean THREAD_DONE[OMP_NTHREADS];
omp_lock_t buffer_lock;
gboolean verbose;
gboolean silent;
gboolean debug;
gboolean central_cut;
gboolean regiao_5l;
gboolean gui_run;
int dist_regiao_5l;
int tam_regiao_5l;
omp_lock_t MC_copy_lock;
Buffer buf;
int *vertexes;
int processadas;
int fsensos,fasensos;
unsigned long nc_bytes_read=0;
long long int readCount_NCUDA = 0;

const int buffer_size_NC = buffer_size;
/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */
#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>	/* POSIX flags */
#include <time.h>	/* clock_gettime(), time() */
#include <sys/time.h>	/* gethrtime(), gettimeofday() */

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#else
#error "Unable to define getRealTimeNC( ) for an unknown OS."
#endif





/**
 * Returns the real time, in seconds, or -1.0 if an error occurred.
 *
 * Time is measured since an arbitrary and OS-dependent start time.
 * The returned real time is only useful for computing an elapsed time
 * between two calls to this function.
 */
double getRealTimeNC( )
{
#if defined(_WIN32)
  FILETIME tm;
  ULONGLONG t;
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
  /* Windows 8, Windows Server 2012 and later. ---------------- */
  GetSystemTimePreciseAsFileTime( &tm );
#else
  /* Windows 2000 and later. ---------------------------------- */
  GetSystemTimeAsFileTime( &tm );
#endif
  t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
  return (double)t / 10000000.0;

#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
  /* HP-UX, Solaris. ------------------------------------------ */
  return (double)gethrtime( ) / 1000000000.0;

#elif defined(__MACH__) && defined(__APPLE__)
  /* OSX. ----------------------------------------------------- */
  static double timeConvert = 0.0;
  if ( timeConvert == 0.0 )
    {
      mach_timebase_info_data_t timeBase;
      (void)mach_timebase_info( &timeBase );
      timeConvert = (double)timeBase.numer /
	(double)timeBase.denom /
	1000000000.0;
    }
  return (double)mach_absolute_time( ) * timeConvert;

#elif defined(_POSIX_VERSION)
  /* POSIX. --------------------------------------------------- */
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  {
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
    /* BSD. --------------------------------------------- */
    const clockid_t id = CLOCK_MONOTONIC_PRECISE;
#elif defined(CLOCK_MONOTONIC_RAW)
    /* Linux. ------------------------------------------- */
    const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_HIGHRES)
    /* Solaris. ----------------------------------------- */
    const clockid_t id = CLOCK_HIGHRES;
#elif defined(CLOCK_MONOTONIC)
    /* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
    const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
    /* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
    const clockid_t id = CLOCK_REALTIME;
#else
    const clockid_t id = (clockid_t)-1;	/* Unknown. */
#endif /* CLOCK_* */
    if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
      return (double)ts.tv_sec +
	(double)ts.tv_nsec / 1000000000.0;
    /* Fall thru. */
  }
#endif /* _POSIX_TIMERS */

  /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. ----- */
  struct timeval tm;
  gettimeofday( &tm, NULL );
  return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#else
  return -1.0;		/* Failed. */
#endif
}
void load_buffer_NONCuda(){
  int i,j;
  int n;
  int loaded;
  if(buf.load == 0){//Se for >0 ainda existem elementos no buffer anterior e se for == -1 não há mais elementos a serem carregados
    readCount_NCUDA += fill_buffer(buf.seq,buf.capacidade,&loaded);//Enche o buffer e guarda a quantidade de sequências carregadas.
    n = strlen(buf.seq[0]);
    for(i=0;i<loaded;i++){	  
      convert_to_graph(buf.seq[i],n,&vertexes[i*n]);
      if( buf.seq[i][0]*(2+buf.seq[i][1]) != vertexes[i*n]){
	printf("Error: %d - %d %s\n",loaded,vertexes[i*n],buf.seq[i]);
	exit(1);
      }
    }
    buf.load = loaded;
  } 
  return;
}

void nc_search_manager(int bloco1,int bloco2,int blocos,const int seqSize_an,Fila *toStore){
  //////////////////////////////////////////
  // Realiza as iteracoes///////////////////
  //////////////////////////////////////////
  
  int *search_gaps;
  int *resultados;
  gboolean retorno;
  int gap;
  int tam;
  int i;
  char *central;
  char *cincol;
  char *seqToSave;
  float iteration_time;
  const int blocoV = blocos-bloco1-bloco2;
  int *candidates;
  Event *hold_event;
  double sec;
  double start_time,end_time;
  processadas = 0;
  fsensos=fasensos=0;
  
  candidates = (int*)malloc(buffer_size_NC*seqSize_an*sizeof(int));
  resultados = (int*)malloc(buffer_size_NC*sizeof(int));
  search_gaps = (int*)malloc(buffer_size_NC*sizeof(int));
  
  load_buffer_NONCuda();
  
  start_time = getRealTimeNC();
  while(buf.load != GATHERING_DONE){
    //Realiza loop enquanto existirem sequências para encher o buffer

    busca(bloco1,bloco2,blocos,&buf,vertexes,candidates,resultados,search_gaps);//Kernel de busca	

	    
    tam = buf.load;
    processadas+= tam;
	    
    for(i = 0; i < tam;i++){
      //Copia sequências senso e antisenso encontradas
      switch(resultados[i]){
      case SENSO:
	if(central_cut){
	  central = (char*)malloc((blocoV+1)*sizeof(char));	
	  gap = search_gaps[i];
	  strncpy(central,buf.seq[i]+gap,blocoV);
	  //printf("%s\n",buf.seq[i]+gap-bloco1);
	  central[blocoV] = '\0';
	}else{				
	  central = (char*)malloc((seqSize_an+1)*sizeof(char));					
	  strncpy(central,buf.seq[i],seqSize_an+1);
	}

	if(regiao_5l){
	  cincol = (char*)malloc((tam_regiao_5l+1)*sizeof(char));

	  gap = search_gaps[i] - dist_regiao_5l;
	  strncpy(cincol,buf.seq[i] + gap,tam_regiao_5l);
	  cincol[tam_regiao_5l] = '\0';
	}else{
	  cincol = NULL;
	}
			      
	fsensos++;
	hold_event = criar_elemento_fila_event(central,cincol,SENSO);
	enfileirar(toStore,hold_event);
	break;
      case ANTISENSO:
	if(central_cut){
	  central = (char*)malloc((blocoV+1)*sizeof(char));
	  gap = search_gaps[i];
	  strncpy(central,buf.seq[i]+gap,blocoV);
	  central[blocoV] = '\0';
	}else{						
	  central = (char*)malloc((seqSize_an+1)*sizeof(char));			
	  strncpy(central,buf.seq[i],seqSize_an+1);
	}

			      
	if(regiao_5l){
	  cincol = (char*)malloc((tam_regiao_5l+1)*sizeof(char));
				      
	  gap = search_gaps[i] + dist_regiao_5l-1;
	  strncpy(cincol,buf.seq[i] + gap,tam_regiao_5l);
	  cincol[tam_regiao_5l] = '\0';
	}else{
	  cincol = NULL;
	}

	fasensos++;
	hold_event = (void*)criar_elemento_fila_event(get_antisenso(central),get_antisenso(cincol),ANTISENSO);
	enfileirar(toStore,hold_event);
	if(central != NULL)
	  free(central);
	if(cincol != NULL)
	  free(cincol);
	break;
      default:
	break;
      }
    }
	    
    // Aguarda o buffer estar cheio novamente
    buf.load = 0;
    load_buffer_NONCuda();
					
    while(tamanho_da_fila(toStore) > LOADER_QUEUE_MAX_SIZE);
  }     
  end_time = getRealTimeNC();
	
  sec = end_time - start_time;
  if(!silent)
    printf("Search kernel executed on %.2f s\n",sec);
  free(resultados);
  free(search_gaps);
  THREAD_DONE[THREAD_SEARCH] = TRUE;
  return;
  //////////////////////////////////////////
  //////////////////////////////////////////
  //////////////////////////////////////////
		
}

void NONcudaIteracoes(int bloco1,int bloco2,int blocos,const int seqSize_an,Socket *gui_socket){
	
  Fila *toStore;
  int blocoV;
  double sec;
  double start_time,end_time;

  //Inicializa
  blocoV = blocos - bloco1 - bloco2+1;
  prepare_buffer(&buf,buffer_size_NC);	
  vertexes = (int*)malloc(buffer_size_NC*seqSize_an*sizeof(int));
  toStore = criar_fila("toStore");
			      
  THREAD_DONE[THREAD_SEARCH] = FALSE;
  THREAD_DONE[THREAD_QUEUE] = FALSE;
  THREAD_DONE[THREAD_DATABASE] = FALSE;
  start_time = getRealTimeNC();
#pragma omp parallel num_threads(OMP_NTHREADS)
  {	
		
#pragma omp sections
    {
#pragma omp section
      {
	// Faz o processamento e adiciona resultado na queue
	nc_search_manager(bloco1,bloco2,blocos,seqSize_an,toStore);
      }
#pragma omp section
      {
	// Carrega resultados da queue e salva no db
	//nc_queue_manager(toStore);
	queue_manager(toStore,&THREAD_DONE[THREAD_SEARCH]);
	THREAD_DONE[THREAD_QUEUE] = TRUE;
      }
#pragma omp section
      {
	// Escrita de informacoes relevantes na stdout
	report_manager(gui_socket,toStore,&processadas,gui_run,verbose,silent,&fsensos,&fasensos,&readCount_NCUDA,&THREAD_DONE[THREAD_QUEUE]);
	THREAD_DONE[THREAD_DATABASE] = TRUE;
      }
    }
  }
  end_time = getRealTimeNC();
  if(!silent){
    sec = end_time-start_time;
    printf("Search executed on %.2f s\n",sec);
  }
  close_buffer(&buf);
  return;
}

void auxNONcuda(char *c,const int bloco1,const int bloco2,const int blocos,Params set,Socket *gui_socket){
	
  int seqSize_an;//Elementos por sequência
  float tempo;

  // Inicializa variaveis
  tempo = 0;
  verbose = set.verbose;
  silent = set.silent;
  debug = set.debug;
  central_cut = set.cut_central;
  gui_run = set.gui_run;
  dist_regiao_5l = set.dist_regiao_5l;
  tam_regiao_5l = set.tam_regiao_5l;
  if(dist_regiao_5l || tam_regiao_5l)
    regiao_5l = TRUE;
  else
    regiao_5l = FALSE;

  printf("CPU mode.\n");
  printString("CPU mode.\n",NULL);
  printf("Buffer size: %d\n",buffer_size);
  printStringInt("Buffer size: ",buffer_size);
	
  seqSize_an = get_setup();
	
  setup_without_cuda(c);
  printSet(seqSize_an);
  printString("Starting hunt...\n",NULL);
	
  NONcudaIteracoes(bloco1,bloco2,blocos,seqSize_an,gui_socket);
    
  printString("Hunt done.",NULL);
  print_tempo(tempo);
	
  return;	
}

