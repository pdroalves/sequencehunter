#include <db.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"


DB *dbp; // DB handler
DB_ENV *envp; // Environment handle
DB_TXN *txn;

void db_start_transaction(){
	int ret; // Retorno
	ret = envp->txn_begin(envp,NULL,&txn,0);
	if (ret != 0) {
        printf("Transaction begin failed.\n");
        exit(1);
    }
   return;
}

void db_commit_transaction(){
	int ret; // Retorno
	ret = txn->commit(txn, 0);
    if (ret != 0) {
        printf("Transaction commit failed.\n");
        exit(1);
    }
	return;
}

void sh_db_create(char *filename){
	int ret; // Retorno
	u_int32_t db_flags; // Flags
	u_int32_t env_flags;
	
	// Abre o environment
	ret = db_env_create(&envp,0);
	if(ret !=0){
		printf("Error creating environment handle: %s\n",db_strerror(ret));
		exit(1);
	}
	
	env_flags = DB_CREATE |    /* Create the environment if it does 
                                * not already exist. */
                DB_INIT_TXN  | /* Initialize transactions */
                DB_INIT_LOCK | /* Initialize locking. */
                DB_INIT_LOG  | /* Initialize logging */
                DB_INIT_MPOOL| /* Initialize the in-memory cache. */
				DB_TXN_NOSYNC|
				DB_TXN_WRITE_NOSYNC;
	
	ret = envp->open(envp,"/home/pedro/",env_flags,0);
	if(ret !=0){
		printf("Error opening environment: %s\n",db_strerror(ret));
		exit(1);
	}
	
	// Abre a database
	ret = db_create(&dbp,envp,0);
	if(ret != 0){
		printf("Couldn't create DB!\n");
		exit(1);
	}
	
	//envp->set_cachesize(dbp,1,3*1e9,1);
	
	
	db_flags = DB_CREATE | DB_AUTO_COMMIT; // Seta a flag
	
	ret = dbp->open(dbp,
					NULL,
					filename,
					NULL,
					 DB_BTREE,
					db_flags,
					0664);
	if(ret != 0){
		printf("Error on DB opening!\n");
		exit(1);
	}
	
	 return;
}

void update_record(Valor *persistente,Valor *atualizacao){
	persistente->qsensos += atualizacao->qsensos;
	persistente->qasensos += atualizacao->qasensos;
	return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret; // Retorno
	DBT key,data;
	Valor new_record;
	
	if(tipo == SENSO){
		new_record.qsensos = 1;
		new_record.qasensos = 0;
	}else{
		new_record.qsensos = 0;
		new_record.qasensos = 1;
	}
	
	// Zera DBTs antes de usa-las
	memset(&key,0,sizeof(DBT));
	memset(&data,0,sizeof(DBT));
	
	key.data = seq_central;
	key.size = strlen(seq_central)*sizeof(char)+1;
	
	data.data = &new_record;
	data.size = sizeof(Valor);
	
	ret = dbp->put(dbp,txn,&key,&data,DB_NOOVERWRITE);
	if(ret == DB_KEYEXIST){
		db_commit_transaction();
		ret = dbp->get(dbp,NULL,&key,&data,0);
		if(ret == 0){
			update_record(data.data,&new_record);
		}else{
			printf("Error on db update!\n");
			exit(1);
		}
		db_start_transaction();
	}
	
	return;
}

void db_destroy(){
	int ret; // Retorno
	if(dbp != NULL)
		dbp->close(dbp,0);
	if(envp != NULL)
		envp->close(envp,0);
	return;
}
