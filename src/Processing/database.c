#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <kchashdb.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"
#if UNDER_CE
#   include <windows.h>
#endif

using namespace std;
using namespace kyotocabinet;

omp_lock_t db_lock;
HashDB db;


extern "C" void db_create ( char *filename,
							 const int key_max_size
							 ){
  // open the database
  if (!db.open(filename, HashDB::OWRITER | HashDB::OCREATE)) {
    cerr << "open error: " << db.error().name() << endl;
  }

}

extern "C" void db_add( char *seq_central,
						 char *seq_cincoL,
						 char *tipo
						 ){

	char *novo;

	novo = (char*)malloc(10*sizeof(char));
	
	if(strcmp(tipo,"S") == 0){
		sprintf(novo,"%d-%d",1,0);
	}else{
		sprintf(novo,"%d-%d",0,1);
	}

  // store records
  if (!db.set(seq_central, novo)) {
    cerr << "set error: " << db.error().name() << endl;
  }
  

}

extern "C" void db_destroy(){

}
