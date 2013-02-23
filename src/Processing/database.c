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

void db_start_transaction(){
	/*		printf("###########################################\n");
			printf("###########################################\n");
			printf("################### NOVA TRANSACTION ###### \n");
			printf("###########################################\n");
			printf("###########################################\n");*/
	sqlite3_exec(db, "BEGIN", 0, 0, 0);
	return;
}

void db_commit_transaction(){
	sqlite3_exec(db, "COMMIT", 0, 0, 0);
	return;
}

void db_create(char *filename){
	int ret;
	
	
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
	char create_table[200] = "CREATE TABLE IF NOT EXISTS events (id INTEGER PRIMARY KEY,main_seq TEXT UNIQUE,qnt_sensos INTEGER DEFAULT 0,qnt_antisensos INTEGER DEFAULT 0,qnt_rel REAL DEFAULT 0.00)";

	// Execute the query for creating the table
	ret = sqlite3_exec(db,create_table,0,0,0);
	
	ret = sqlite3_exec(db,"PRAGMA synchronous=OFF",0,0,0);
	ret = sqlite3_exec(db,"PRAGMA setAutocommit(false)",0,0,0);
	ret = sqlite3_exec(db,"PRAGMA page_size = 4096",0,0,0);
	ret = sqlite3_exec(db,"PRAGMA journal_size_limit = 104857600",0,0,0);
	ret = sqlite3_exec(db,"PRAGMA count_changes = OFF",0,0,0);
	ret = sqlite3_exec(db,"PRAGMA cache_size = 10000",0,0,0);
	
	query = (char*)malloc(q_size*sizeof(char));
	count =0;
	 return;
}

void db_add(char *seq_central,char *seq_cincoL,int tipo){
	int ret;
	int cols;
	char *id;
	sqlite3_stmt *stmt;
	
	id = (char*)malloc(10*sizeof(char));

	sprintf(id,"%d",count);
	
	// Verifica a existencia da seq_central no db
	strcpy(query,"SELECT EXISTS(SELECT '");
	strcat(query,seq_central);
	strcat(query,"' FROM events LIMIT 1");
	
	ret = sqlite3_prepare_v2(db,query,-1,&stmt,0);
	
	cols = sqlite3_column_count(stmt);;
	if(cols > 0){
		// Monta query para update
		strcpy(query,"UPDATE events SET");
		if(strcmp(tipo,"S") == 0){
			strcat(query,"qnt_sensos=qnt_sensos+1 WHERE main_seq='");
		}else{
			strcat(query,"qnt_antisensos=qnt_antisensos+1 WHERE main_seq='");			
		}
		strcat(query,seq_central);
		strcat(query,"'");
		
	}else{	
		// Monta query para insert
		strcpy(query,"INSERT INTO events (id,main_seq,");
		
		if(tipo == SENSO){		
			strcat(query,"qnt_sensos) VALUES(");
		}else{
			strcat(query,"qnt_antisensos) VALUES('");
		}
		strcat(query,id);
		strcat(query,",'");
		strcat(query,seq_central);
		strcat(query,"',");
		strcat(query,"1);");
		count++;
			
	}

	// Aplica query
	//printf("%s\n",query);
	ret = sqlite3_exec(db,query,0,0,0);
	return;
}

void db_destroy(){
	sqlite3_close(db);
	free(query);
}
