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
	char create_table[200] = "CREATE TABLE IF NOT EXISTS events (main_seq TEXT NOT NULL UNIQUE,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,qnt_rel REAL DEFAULT 0.00)";

	// Execute the query for creating the table
	ret = sqlite3_exec(db,create_table,NULL, NULL,&sErrMsg);
	
	// Setup DB
	sqlite3_exec(db,"PRAGMA synchronous = OFF", NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	sqlite3_exec(db,"PRAGMA journal_mode = OFF",NULL,NULL,&sErrMsg);
	if(sErrMsg != NULL){
		printf("Pragma error: %s\n",sErrMsg);
		exit(1);
	}
	
	count =0;
	
	insertSQL = (char*)malloc(500*sizeof(char));
	
	// Compile insert-statement
	// Sensos
	sprintf(insertSQL, "INSERT OR REPLACE INTO events (main_seq,qnt_sensos,qnt_antisensos) VALUES (@SEQ,COALESCE((SELECT qnt_sensos FROM events WHERE main_seq=@SEQ)+1,1),(SELECT qnt_antisensos FROM events WHERE main_seq=@SEQ))");
	ret = sqlite3_prepare_v2(db,  insertSQL, -1, &stmt_senso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile - %d.\n",ret);
		exit(1);
	}
	
	// Antisensos
	sprintf(insertSQL, "INSERT OR REPLACE INTO events (main_seq,qnt_sensos,qnt_antisensos) VALUES (@SEQ,(SELECT qnt_sensos FROM events WHERE main_seq=@SEQ),COALESCE((SELECT qnt_antisensos FROM events WHERE main_seq=@SEQ)+1,1))");
	ret = sqlite3_prepare_v2(db,  insertSQL, -1, &stmt_antisenso, 0);
	if(ret != SQLITE_OK){
		printf("Error on statement compile - %d.\n",ret);
		exit(1);
	}
	
	 return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret=0;
	int cols;
    char * sErrMsg;
    
    
    if(tipo == SENSO){
		sqlite3_reset(stmt_senso);
		sqlite3_bind_text(stmt_senso,1,seq_central,-1,SQLITE_TRANSIENT);	
		ret = sqlite3_step(stmt_senso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 1 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}else{
		sqlite3_reset(stmt_antisenso);	
		sqlite3_bind_text(stmt_antisenso,1,seq_central,-1,SQLITE_TRANSIENT);
		ret = sqlite3_step(stmt_antisenso);
		if(ret != SQLITE_DONE){
			printf("Error on SQLite step 2 - %d. => %s\n",ret,seq_central);
			exit(1);
		}
	}
	
	count++;
	return;
}

void db_destroy(){
	sqlite3_finalize(stmt_senso);
	sqlite3_finalize(stmt_antisenso);
	sqlite3_close(db);
}
