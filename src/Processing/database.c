#include <ham/hamsterdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

ham_db_t *db; /* hamsterdb database object */
ham_env_t* env;
ham_txn_t *txn;
ham_cursor_t *cursor;
omp_lock_t db_lock;

#define MAX_KEY_SIZE 21

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

void db_init_lock(){
	omp_init_lock(&db_lock);
}

void db_start_transaction(){
	ham_status_t st;       /* status variable */
	// create a new transaction
	st = ham_txn_begin(&txn, env,"txn",NULL, 0);
	 if (st!=HAM_SUCCESS){
		error("ham_txn_begin", st);
		exit(1);
	}
}

void db_commit_transaction(){
	ham_status_t st;       /* status variable */
	st=ham_txn_commit(txn, 0);
	if (st!=HAM_SUCCESS) {
		printf("ham_txn_commit failed: %d (%s)\n", st, ham_strerror(st));
		exit(-1);
	}
}

void db_create(char *filename,const int key_max_size){
	ham_status_t st;       /* status variable */
	//db_count = fopen("db_count.dat","w+");
	txn = NULL;
	const ham_parameter_t params_env[] = {
								{HAM_PARAM_KEYSIZE,key_max_size},
								{HAM_PARAM_CACHESIZE,100*1024*1024},
								{HAM_PARAM_PAGESIZE,2048},
								 {0,NULL} };
	const ham_parameter_t params_main_db[] = {
								{HAM_PARAM_KEYSIZE,key_max_size},
								 {0,NULL} };
							 
	// Environment
	st = ham_env_new(&env);
	 if (st!=HAM_SUCCESS){
		error("ham_env_new", st);
		exit(1);
	}
	
	st = ham_env_create_ex(env, filename, HAM_ENABLE_TRANSACTIONS, 0664, params_env);

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
	
	 st=ham_env_create_db(env, db, 1, 0, params_main_db);
	 if (st!=HAM_SUCCESS){
		error("ham_env_create_db", st);
		exit(1);
	}
	
	if ((st=ham_cursor_create(db, txn, 0, &cursor))) {
		error("ham_cursor_create", st);
		exit(1);
	}
	 return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	ham_status_t st;       /* status variable */
	ham_key_t key;         /* the structure for a key */
	ham_record_t record;   /* the structure for a record */
	char *main_key;
	char *overflow_key;
	
	Valor *v;	
	
	memset(&key, 0, sizeof(key));
	memset(&record, 0, sizeof(record));
	   
	key.data=seq_central;
	key.size=strlen(seq_central)*sizeof(char)+1;
	
	/*// Verifica se a chave jah estah contida no db
	st = ham_cursor_find(cursor,&key,HAM_FIND_EXACT_MATCH);
	if(st == HAM_SUCCESS){
		// Esta contida e old_record estah atualizado
		ham_cursor_move(cursor,&key,&record,0);
		v = (Valor*) record.data;
		if(tipo == SENSO){
			v->qsensos++;
		}else if(tipo == ANTISENSO){
			v->qasensos++;
		}else{
			return;
		}
	}else{*/
		// Nao estah contida
		/*v = (Valor*)malloc(sizeof(Valor));
		
		if(tipo == SENSO){
			v->qsensos = 1;
			v->qasensos = 0;
		}else if(tipo == ANTISENSO){
			v->qsensos = 0;
			v->qasensos = 1;		
		}else{
			return;
		}*/
		
		record.data="Oi";
		record.size=2*sizeof(char)+1;
		
		st=ham_cursor_insert(cursor,&key, &record, HAM_OVERWRITE);
		if(st != HAM_SUCCESS){
			error("ham_cursor_insert",st);
			exit(1);
		}
	//}
	
 
	return;
}

void db_destroy(){
	ham_cursor_close(cursor);
	ham_close(db, 0);
	ham_delete(db);
}
