#if UNDER_CE
#   include <windows.h>
#endif
#include <ham/hamsterdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

void
error(const char *foo, ham_status_t st)
{
    printf("%s() returned error %d: %s\n", foo, st, ham_strerror(st));
    exit(-1);
}

 static int 
 my_string_compare(ham_db_t *db, const ham_u8_t *lhs, ham_size_t lhs_length, 
                   const ham_u8_t *rhs, ham_size_t rhs_length)
 {
     (void)db;
 
     return strncmp((const char *)lhs, (const char *)rhs, 
            lhs_length<rhs_length ? lhs_length : rhs_length);
 }



extern "C" ham_db_t* ham_create_db(char *filename,const int key_max_size){
	ham_db_t *db; /* hamsterdb database object */
	ham_status_t st;       /* status variable */
	
    const ham_parameter_t params[] = {
								{HAM_PARAM_KEYSIZE,key_max_size},
								{HAM_PARAM_PAGESIZE,1024},
								 {0,NULL}
							 };
    
     st=ham_new(&db); // New db object
     if (st!=HAM_SUCCESS){
        error("ham_new", st);
        exit(1);
	}
	
	 st=ham_set_compare_func(db, my_string_compare);
     if (st) {
       printf("ham_set_compare_func() failed with error %d\n", st);
		exit (-1);
     }
	
     st=ham_create_ex(db, filename, HAM_CACHE_UNLIMITED, 0664, params);
     //st=ham_create_ex(db, NULL, HAM_IN_MEMORY_DB, 0664, 0);
     if (st!=HAM_SUCCESS){
        error("ham_create_ex", st);
        exit(1);
	}
     return db;
}

extern "C" void ham_add(ham_db_t* db,char *seq_central,char *seq_cincoL,char *tipo){
	ham_status_t st;       /* status variable */
    ham_key_t key;         /* the structure for a key */
    ham_record_t record;   /* the structure for a record */
	Valor *v;	
	
	
    
	memset(&key, 0, sizeof(key));
	memset(&record, 0, sizeof(record));
   
    key.data=seq_central;
    key.size=strlen(seq_central)*sizeof(char)+1;
	
	// Verifica se a chave jah estah contida no db
	if(ham_find(db,NULL,&key,&record,0) != HAM_KEY_NOT_FOUND){
		// Esta contida e old_record estah atualizado
		v = (Valor*) record.data;
		if(strcmp(tipo,"S")){
			v->qsensos++;
		}else if(strcmp(tipo,"AS")){
			v->qasensos++;
		}else{
			return;
		}
	}else{
		// Nao estah contida
		v = (Valor*)malloc(sizeof(Valor));
	
		if(strcmp(tipo,"S")){
			v->qsensos = 1;
			v->qasensos = 0;
		}else if(strcmp(tipo,"AS")){
			v->qsensos = 0;
			v->qasensos = 1;		
		}else{
			return;
		}
		
		v->qpares = 0;
		v->qrlv = 0;
		
		record.data=v;
		record.size=sizeof(Valor)+1;
		
		st=ham_insert(db, 0, &key, &record, 0);
		if(st != HAM_SUCCESS){
			error("ham_insert",st);
			exit(1);
		}
	}
	
	
 
	return;
}

extern "C" void ham_read(ham_db_t* db){
	int i;
	ham_status_t st;       /* status variable */
    ham_key_t key;         /* the structure for a key */
    ham_record_t record;   /* the structure for a record */
	for (i=0; i<10; i++) {
        key.data=&i;
        key.size=sizeof(i);

        st=ham_find(db, 0, &key, &record, 0);

        /*
         * check if the value is ok
         */
        if (*(int *)record.data!=i) {
            printf("ham_find() ok, but returned bad value\n");
        }
    }
}

extern "C" void ham_destroy(ham_db_t* db){
	ham_close(db, 0);
    ham_delete(db);
}
