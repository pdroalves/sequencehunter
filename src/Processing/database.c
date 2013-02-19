#if UNDER_CE
#   include <windows.h>
#endif
#include <ham/hamsterdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

ham_env_t* env;

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
								 {0,NULL}
							 };
							 
	// Environment
	st = ham_env_new(&env);
     if (st!=HAM_SUCCESS){
        error("ham_env_new", st);
        exit(1);
	}
	
	st = ham_env_create_ex(env, filename, 0, 0664, params);

     if (st!=HAM_SUCCESS){
        error("ham_env_create_ex", st);
        exit(1);
	}
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
	
     st=ham_env_create_db(env, db, 1, 0, params);
     if (st!=HAM_SUCCESS){
        error("ham_env_create_db", st);
        exit(1);
	}
     return db;
}

extern "C" void ham_add(ham_db_t* db,char *seq_central,char *seq_cincoL,char *tipo){
	ham_status_t st;       /* status variable */
    ham_key_t key;         /* the structure for a key */
    ham_record_t record;   /* the structure for a record */
    ham_txn_t *txn; 
	
	Valor *v;	
    
	memset(&key, 0, sizeof(key));
	memset(&record, 0, sizeof(record));
   
    key.data=seq_central;
    key.size=strlen(seq_central)*sizeof(char)+1;
	
	// Verifica se a chave jah estah contida no db
	st = ham_find(db,NULL,&key,&record,0);
	if(st != HAM_KEY_NOT_FOUND){
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
		
		st=ham_insert(db, NULL, &key, &record, 0);
		if(st != HAM_SUCCESS){
			error("ham_insert",st);
			exit(1);
		}
	}
 
	return;
}

extern "C" void ham_destroy(ham_db_t* db){
	ham_close(db, 0);
    ham_delete(db);
}
