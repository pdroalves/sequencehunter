#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#ifdef _WIN32
#include "C:\SQLite3\sqlite-source\sqlite3.h"
#endif
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

#define GIGA 1073741824 
#define DB_FREE_MEMORY_FACTOR 0.95
#define q_size 500
#define DATABASE ":memory:"

const int MAX_DB_MEM_USE_GB = 2;
// Create a db for database connection, create a pointer to sqlite3
sqlite3 *db;
// The number of query to be dbd,size of each query and pointer
int count;
sqlite3_stmt *stmt_insert;
sqlite3_stmt *stmt_update_senso;
sqlite3_stmt *stmt_update_antisenso;

void db_start_transaction(){
    char * sErrMsg;
	/*		printf("###########################################\n");
			printf("###########################################\n");
			printf("################### NOVA TRANSACTION ###### \n");
			printf("###########################################\n");
			printf("###########################################\n");*/
	sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, &sErrMsg);
	return;
}

void db_commit_transaction(){
	float memory_used_limit = MAX_DB_MEM_USE_GB*DB_FREE_MEMORY_FACTOR;
	char *sErrMsg;
	sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, &sErrMsg);
	if(sqlite3_memory_used()/(float)GIGA > memory_used_limit)
		sqlite3_db_release_memory(db);
	return;
}

void db_create(char *filename){
	int ret;
    char * sErrMsg;
	char *query;
	
	ret = sqlite3_open(filename,&db);
	
    if (!db)
        printf("Not sure why, but the database didn't open.\n");
        
	// If connection failed, db returns NULL
	if(ret)
	{
		printf("Database connection failed\n");
		exit(1);
	}
	
	printf("Connection successful\n");

	// Setup DB
	sqlite3_exec(db,"PRAGMA synchronous = 1", NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA journal_mode = OFF",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}	
	sqlite3_exec(db,"PRAGMA page_size = 4096",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA cache_size = 1000000",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA shrink_memory",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}	
	
		
	query = (char*)malloc(500*sizeof(char));
	
	// Create the SQL query for creating a table
	strcpy(query,"CREATE TABLE events (main_seq TEXT NOT NULL PRIMARY KEY UNIQUE,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,pares INTEGER DEFAULT 0)");

	// Execute the query for creating the table
	ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);

	count =0;
	
	// Compile insert-statement
	sprintf(query, "INSERT OR IGNORE INTO events (main_seq) VALUES (@SEQ)");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_insert, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 1 - %d.\n",ret);
		exit(1);
	}

	// Sensos
	sprintf(query, "UPDATE events SET qnt_sensos=qnt_sensos+1 WHERE main_seq = @SEQ");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_update_senso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 2 - %d.\n",ret);
		exit(1);
	}
	
	// Antisensos
	sprintf(query, "UPDATE events SET qnt_antisensos=qnt_antisensos+1 WHERE main_seq = @SEQ");
	ret = sqlite3_prepare_v2(db,  query, -1, &stmt_update_antisenso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 3 - %d.\n",ret);
		exit(1);
	}
	
	sqlite3_soft_heap_limit64(MAX_DB_MEM_USE_GB*GIGA);
	
	free(query);
	return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret=0;
    
	sqlite3_bind_text(stmt_insert,1,seq_central,-1,SQLITE_STATIC);
	ret = sqlite3_step(stmt_insert);	
	sqlite3_clear_bindings(stmt_insert);
	sqlite3_reset(stmt_insert);
	if(ret != SQLITE_DONE){
		printf("Error on SQLite step 1 - %d. => %s\n",ret,seq_central);
		exit(1);
	}
    
    if(tipo == SENSO){
		sqlite3_bind_text(stmt_update_senso,1,seq_central,-1,SQLITE_STATIC);
		ret = sqlite3_step(stmt_update_senso);	
        sqlite3_clear_bindings(stmt_update_senso);
		sqlite3_reset(stmt_update_senso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 2 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}else{
		sqlite3_bind_text(stmt_update_antisenso,1,seq_central,-1,SQLITE_STATIC);
		ret = sqlite3_step(stmt_update_antisenso);
		sqlite3_reset(stmt_update_antisenso);	
		sqlite3_clear_bindings(stmt_update_antisenso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 3 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}
	
	count++;
	return;
}

void db_destroy(){
    char * sErrMsg;
	int ret;
	char query[] = "UPDATE events SET pares = min(qnt_sensos,qnt_antisensos)";

	printf("Limpando\n");
	
	ret = sqlite3_exec(db,query,NULL, NULL,&sErrMsg);
	sqlite3_finalize(stmt_update_senso);
	sqlite3_finalize(stmt_update_antisenso);
	sqlite3_finalize(stmt_insert);
	sqlite3_close(db);
}
