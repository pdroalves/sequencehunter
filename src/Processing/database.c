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
	
	
	flags = DB_CREATE; // Seta a flag
	
	ret = dbp->open(dbp,
					NULL,
					filename,
					NULL,
					DB_BTREE,
					flags,
					0664);
	if(ret != 0){
		printf("Error on DB opening!\n");
		exit(1);
	}
	
	 return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret; // Retorno
	DBT key,data;
	Valor record;
	
	if(tipo == SENSO){
		record.qsensos = 1;
		record.qasensos = 0;
	}else{
		record.qsensos = 0;
		record.qasensos = 1;
	}
	
	// Zera DBTs antes de usa-las
	memset(&key,0,sizeof(DBT));
	memset(&data,0,sizeof(DBT));
	
	key.data = seq_central;
	key.size = strlen(seq_central)*sizeof(char)+1;
	
	data.data = &record;
	data.size = sizeof(record);
	
	ret = dbp->put(dbp,NULL,&key,&data,0);
	if(ret == DB_KEYEXIST){
		printf("Error on adding data!\n");
		exit(1);
	}
	
	return;
}

void db_destroy(){
	int ret; // Retorno
	
}
