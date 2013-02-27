#include <db.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"


DB *dbp; // DB handler

void db_start_transaction(){
	int ret; // Retorno
   return;
}

void db_commit_transaction(){
	int ret; // Retorno
	return;
}

void sh_db_create(char *filename){
	int ret; // Retorno
	u_int32_t flags; // Flags
	
	ret = db_create(&dbp,NULL,0);
	if(ret != 0){
		printf("Couldn't create DB!\n");
		exit(1);
	}
	
	dbp->set_cachesize(dbp,1,3*1e9,1);
	
	
	flags = DB_CREATE; // Seta a flag
	
	ret = dbp->open(dbp,
					NULL,
					filename,
					NULL,
					DB_HASH,
					flags,
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
	
	ret = dbp->put(dbp,NULL,&key,&data,DB_NOOVERWRITE);
	if(ret == DB_KEYEXIST){
		ret = dbp->get(dbp,NULL,&key,&data,0);
		if(ret == 0){
			update_record(data.data,&new_record);
		}else{
			printf("Error on db update!\n");
			exit(1);
		}
	}
	
	return;
}

void db_destroy(){
	int ret; // Retorno
	if(dbp != NULL)
		dbp->close(dbp,0);
}
