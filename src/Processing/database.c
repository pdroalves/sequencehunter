#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "../Headers/estruturas.h"
#include "../Headers/load_data.h"

#define q_size 500
// Create a db for database connection, create a pointer to sqlite3
sqlite3 *db;
// The number of query to be dbd,size of each query and pointer
char *query;
int count;
char *insertSQL;
sqlite3_stmt *stmt;

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
	char create_table[200] = "CREATE TABLE IF NOT EXISTS events (main_seq TEXT UNIQUE,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,qnt_rel REAL DEFAULT 0.00)";

	// Execute the query for creating the table
	ret = sqlite3_exec(db,create_table,0,0,&sErrMsg);
	
	// Setup DB
	sqlite3_prepare_v2(db,"PRAGMA synchronous = OFF", -1, &stmt, 0);
	sqlite3_prepare_v2(db,"PRAGMA journal_mode = OFF",-1,&stmt,0);
	
	query = (char*)malloc(q_size*sizeof(char));
	count =0;
	
	insertSQL = (char*)malloc(500*sizeof(char));
	
	// Compile insert-statement
	sprintf(insertSQL, "INSERT INTO TTC VALUES (@SEQ,@SENSOS,@ASENSOS)");
	sqlite3_prepare_v2(db,  insertSQL, -1, &stmt, 0);
	 return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret;
	int cols;
    char * sErrMsg;
    char *sSEQ;
    char *sSENSOS;
    char *sASENSOS;
	
	// Verifica a existencia da seq_central no db
	sprintf(query,"SELECT EXISTS(SELECT '%s' FROM events LIMITE 1)",seq_central);
	
	ret = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	cols = sqlite3_column_count(stmt);;
	if(cols > 0){
		// Monta query para update
		if(tipo == SENSO){
			sprintf(query,"UPDATE events SET qnt_sensos=qnt_sensos+1 WHERE main_seq='%s'");
		}else{	
			sprintf(query,"UPDATE events SET qnt_antisensos=qnt_antisensos+1 WHERE main_seq='%s'");
		}
	}else{	
		// Monta query para insert	
		if(tipo == SENSO){		
			sqlite3_bind_text(stmt,0,seq_central,-1,SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt,1,1);
			sqlite3_bind_int(stmt,2,0);
		}else{
			sqlite3_bind_text(stmt,0,seq_central,-1,SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt,1,0);
			sqlite3_bind_int(stmt,2,1);
		}
		
		count++;
			
	}

	// Aplica query
	//printf("%s\n",query);
	//ret = sqlite3_exec(db,query,0,0,&sErrMsg);
	
	 sqlite3_step(stmt);

    //sqlite3_clear_bindings(stmt);
   
	return;
}

void db_destroy(){
	sqlite3_close(db);
	free(query);
}
