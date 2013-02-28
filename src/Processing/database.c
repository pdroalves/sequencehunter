#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

#define q_size 500
#define DATABASE ":memory:"
// Create a db for database connection, create a pointer to sqlite3
sqlite3 *db;
// The number of query to be dbd,size of each query and pointer
int count;
char *insertSQL;
sqlite3_stmt *stmt_seq;
sqlite3_stmt *stmt_senso;
sqlite3_stmt *stmt_antisenso;

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
	char *sErrMsg;
	sqlite3_exec(db, "COMMIT TRANSACTION", 0, 0, &sErrMsg);
	return;
}

void db_create(char *filename){
	int ret;
    char * sErrMsg;
    sqlite3_stmt *stmt;
	
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
	
	// Create the SQL query for creating a table
	char create_table[200] = "CREATE TABLE events (main_seq TEXT NOT NULL PRIMARY KEY UNIQUE,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,qnt_rel REAL DEFAULT 0.00)";

	// Execute the query for creating the table
	ret = sqlite3_exec(db,create_table,NULL, NULL,&sErrMsg);
	
	// Setup DB
	sqlite3_exec(db,"PRAGMA synchronous = ON", NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA journal_mode = MEMORY",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}	
	sqlite3_exec(db,"PRAGMA cache_size = 1000000",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA ignore_check_constarints = true",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}	

	count =0;
	
	insertSQL = (char*)malloc(500*sizeof(char));
	
	// Compile insert-statement
	sprintf(insertSQL, "INSERT OR IGNORE INTO events (main_seq) VALUES (@SEQ)");
	ret = sqlite3_prepare_v2(db,  insertSQL, -1, &stmt_seq, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 1 - %d.\n",ret);
		exit(1);
	}

	// Sensos
	sprintf(insertSQL, "UPDATE events SET qnt_sensos=qnt_sensos+1 WHERE main_seq LIKE @SEQ;");
	ret = sqlite3_prepare_v2(db,  insertSQL, -1, &stmt_senso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 2 - %d.\n",ret);
		exit(1);
	}
	
	// Antisensos
	sprintf(insertSQL, "UPDATE events SET qnt_antisensos=qnt_antisensos+1 WHERE main_seq LIKE @SEQ;");
	ret = sqlite3_prepare_v2(db,  insertSQL, -1, &stmt_antisenso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile 3 - %d.\n",ret);
		exit(1);
	}
	 return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret=0;
	int cols;
    char * sErrMsg;
    
	sqlite3_bind_text(stmt_seq,1,seq_central,-1,SQLITE_TRANSIENT);
	ret = sqlite3_step(stmt_seq);	
	sqlite3_clear_bindings(stmt_seq);
	sqlite3_reset(stmt_seq);
	if(ret != SQLITE_DONE){
		printf("Error on SQLite step 1 - %d. => %s\n",ret,seq_central);
		exit(1);
	}
    
   /* if(tipo == SENSO){
		sqlite3_bind_text(stmt_senso,1,seq_central,-1,SQLITE_TRANSIENT);
		ret = sqlite3_step(stmt_senso);	
        sqlite3_clear_bindings(stmt_senso);
		sqlite3_reset(stmt_senso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 2 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}else{
		sqlite3_bind_text(stmt_antisenso,1,seq_central,-1,SQLITE_TRANSIENT);
		ret = sqlite3_step(stmt_antisenso);
		sqlite3_reset(stmt_antisenso);	
		sqlite3_clear_bindings(stmt_antisenso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 3 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}*/
	
	count++;
	return;
}

void db_destroy(){
	sqlite3_finalize(stmt_senso);
	sqlite3_finalize(stmt_antisenso);
	sqlite3_close(db);
}
